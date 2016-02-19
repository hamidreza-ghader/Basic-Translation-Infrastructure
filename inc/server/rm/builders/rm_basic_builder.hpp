/* 
 * File:   rm_basic_builder.hpp
 * Author: Dr. Ivan S. Zapreev
 *
 * Visit my Linked-in profile:
 *      <https://nl.linkedin.com/in/zapreevis>
 * Visit my GitHub:
 *      <https://github.com/ivan-zapreev>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.#
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Created on February 8, 2016, 9:56 AM
 */

#ifndef RM_BASIC_BUILDER_HPP
#define RM_BASIC_BUILDER_HPP

#include <cmath>

#include "common/utils/exceptions.hpp"
#include "common/utils/logging/logger.hpp"
#include "common/utils/file/text_piece_reader.hpp"
#include "common/utils/string_utils.hpp"

#include "server/common/models/phrase_uid.hpp"
#include "server/rm/rm_parameters.hpp"
#include "server/rm/models/rm_entry.hpp"

using namespace std;

using namespace uva::utils::exceptions;
using namespace uva::utils::logging;
using namespace uva::utils::file;
using namespace uva::utils::text;

using namespace uva::smt::bpbd::server::rm;
using namespace uva::smt::bpbd::server::rm::models;
using namespace uva::smt::bpbd::server::common::models;

namespace uva {
    namespace smt {
        namespace bpbd {
            namespace server {
                namespace rm {
                    namespace builders {

                        //Stores the translation model delimiter character for parsing one line
                        static const char RM_DELIMITER = '|';
                        //Stores the translation model delimiter character cardinality
                        static const size_t RM_DELIMITER_CDTY = 3;

                        /**
                         * This class represents a basic reader of the reordering model.
                         * It allows to read a text-formatted reordering model and to put
                         * it into the given instance of the model class. It assumes the
                         * simple text model format as used by Oyster or Moses.
                         * See http://www.statmt.org/moses/?n=Moses.Tutorial for some info.
                         * The reordering model is also commonly known as a phrase table.
                         */
                        template< typename model_type, typename reader_type>
                        class rm_basic_builder {
                        public:

                            /**
                             * The basic constructor of the builder object
                             * @params params the model parameters
                             * @param model the model to put the data into
                             * @param reader the reader to read the data from
                             */
                            rm_basic_builder(const rm_parameters & params, model_type & model, reader_type & reader)
                            : m_params(params), m_model(model), m_reader(reader) {
                            }

                            /**
                             * Allows to build the model by reading from the reader object.
                             * This is a two step process as first we need the number
                             * of distinct source phrases.
                             */
                            void build() {
                                //Count and set the number of source phrases if needed
                                if (m_model.is_num_entries_needed()) {
                                    set_number_source_phrases();
                                }

                                //Process the translations
                                process_source_entries();
                            }

                        protected:

                            /**
                             * Allows to count and set the number of source phrases
                             */
                            inline void set_number_source_phrases() {
                                Logger::start_progress_bar(string("Counting reordering entries"));

                                //Declare the text piece reader for storing the read line and source phrase
                                TextPieceReader line, source;
                                //Stores the number of reordering entries for logging
                                size_t num_entries = 0;

                                //Compute the number of reordering entries. Note that,
                                //any new line is just an extra entry so any empty line
                                //will just insignificantly increase the memory consumption.
                                while (m_reader.get_first_line(line)) {
                                    //Increment the counter
                                    ++num_entries;

                                    //Update the progress bar status
                                    Logger::update_progress_bar();
                                }
                                LOG_DEBUG << "Counter the number of reordering entries: " << num_entries << END_LOG;

                                //Set the number of entries into the model
                                m_model.set_num_entries(num_entries);

                                //Re-set the reader to start all over again
                                m_reader.reset();

                                //Stop the progress bar in case of no exception
                                Logger::stop_progress_bar();
                            }

                            /**
                             * Allows to parse the reordering weights and set them into the reordering entry
                             * @param rest the line to be parsed, starts with a space
                             * @param entry the entry to put the values into
                             */
                            void process_entry_weights(TextPieceReader & rest, rm_entry & entry) {
                                //Declare the token to store weights
                                TextPieceReader token;

                                //Skip the first space
                                ASSERT_CONDITION_THROW(!rest.get_first_space(token), "Could not skip the first space!");

                                //Read the subsequent weights, check that the number of weights is as expected
                                size_t idx = 0;
                                while (rest.get_first_space(token) && (idx < rm_entry::NUM_FEATURES)) {
                                    //Parse the token into the entry weight
                                    fast_s_to_f(entry[idx], token.str().c_str());
                                    //Now convert to the log probability and multiply with the appropriate weight
                                    entry[idx] = log10(entry[idx]) * m_params.rm_weights[idx];
                                    //Increment the index 
                                    ++idx;
                                }
                            }

                            /**
                             * Allows to process translations.
                             */
                            void process_source_entries() {
                                Logger::start_progress_bar(string("Building reordering model"));

                                //Declare the text piece reader for storing the read line and source phrase
                                TextPieceReader line, source, target;

                                //Store the cached source string and its uid values
                                string source_str = "";
                                phrase_uid source_uid = UNDEFINED_PHRASE_ID;
                                phrase_uid target_uid = UNDEFINED_PHRASE_ID;

                                //Start reading the translation model file line by line
                                while (m_reader.get_first_line(line)) {
                                    //Read the source phrase
                                    line.get_first<RM_DELIMITER, RM_DELIMITER_CDTY>(source);

                                    //Get the current source phrase uids
                                    string next_source_str = source.str();
                                    trim(next_source_str);
                                    if (source_str != next_source_str) {
                                        //Store the new source string
                                        source_str = next_source_str;
                                        //Compute the new source string uid
                                        source_uid = get_phrase_uid(source_str);
                                    }

                                    //Read the target phrase
                                    line.get_first<RM_DELIMITER, RM_DELIMITER_CDTY>(target);
                                    string target_str = target.str();
                                    trim(target_str);

                                    LOG_DEBUG << "Got rm entry: " << source_str << " / " << target_str << END_LOG;

                                    //Parse the rest of the target entry
                                    target_uid = get_phrase_uid<true>(target_str);
                                    process_entry_weights(line, m_model.add_entry(source_uid, target_uid));

                                    //Update the progress bar status
                                    Logger::update_progress_bar();
                                }

                                //Find the UNK entry, this is needed for performance optimization.
                                m_model.find_unk_entry();

                                //Stop the progress bar in case of no exception
                                Logger::stop_progress_bar();
                            }

                        private:
                            //Stores the reference to the model parameters
                            const rm_parameters & m_params;
                            //Stores the reference to the model
                            model_type & m_model;
                            //Stores the reference to the builder;
                            reader_type & m_reader;
                        };
                    }
                }
            }
        }
    }
}

#endif /* RM_BASIC_BUILDER_HPP */

