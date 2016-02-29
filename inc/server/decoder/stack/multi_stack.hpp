/* 
 * File:   trans_stack.hpp
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
 * Created on February 16, 2016, 4:20 PM
 */

#ifndef MULTI_STACK_HPP
#define MULTI_STACK_HPP

#include <string>
#include <functional>

#include "common/utils/threads.hpp"
#include "common/utils/exceptions.hpp"
#include "common/utils/logging/logger.hpp"

#include "server/lm/lm_configurator.hpp"
#include "server/rm/proxy/rm_query_proxy.hpp"

#include "server/decoder/de_configs.hpp"
#include "server/decoder/de_parameters.hpp"
#include "server/decoder/sentence/sentence_data_map.hpp"
#include "server/decoder/stack/stack_level.hpp"

using namespace std;
using namespace std::placeholders;

using namespace uva::utils::threads;
using namespace uva::utils::logging;
using namespace uva::utils::exceptions;

using namespace uva::smt::bpbd::server::lm;
using namespace uva::smt::bpbd::server::lm::proxy;
using namespace uva::smt::bpbd::server::rm::proxy;

using namespace uva::smt::bpbd::server::decoder;
using namespace uva::smt::bpbd::server::decoder::sentence;

namespace uva {
    namespace smt {
        namespace bpbd {
            namespace server {
                namespace decoder {
                    namespace stack {

                        //Stores the number of extrs stack levels that we will need in a multi-
                        //stack, the first one is for <s> and the second one is for </s>
                        static constexpr uint32_t NUM_EXTRA_STACK_LEVELS = 2;

                        //Stores the minimum stack level index
                        static constexpr uint32_t MIN_STACK_LEVEL = 0;

                        /**
                         * This is the translation stack class that is responsible for the sentence translation
                         */
                        class multi_stack {
                        public:

                            /**
                             * The basic constructor
                             * @param params the decoder parameters, stores the reference to it
                             * @param is_stop the stop flag
                             * @param sent_data the retrieved sentence data
                             * @param rm_query the reordering model query
                             * @param lm_query the language model query object
                             */
                            multi_stack(const de_parameters & params,
                                    acr_bool_flag is_stop,
                                    const sentence_data_map & sent_data,
                                    const rm_query_proxy & rm_query,
                                    lm_fast_query_proxy & lm_query)
                            : m_params(params), m_is_stop(is_stop), m_sent_data(sent_data),
                            m_rm_query(rm_query), m_lm_query(lm_query),
                            m_num_levels(m_sent_data.get_dim() + NUM_EXTRA_STACK_LEVELS),
                            m_curr_level(MIN_STACK_LEVEL) {
                                LOG_DEBUG1 << "Created a multi stack with parameters: " << m_params << END_LOG;

                                //Instantiate an array of stack level pointers
                                m_levels = new stack_level_ptr[m_num_levels]();

                                //Initialize the stack levels
                                for (uint32_t level = MIN_STACK_LEVEL; level < m_num_levels; ++level) {
                                    m_levels[level] = new stack_level(m_params, m_is_stop);
                                }

                                //Add the root state to the stack, the root state must have 
                                //information about the sentence data, rm and lm query and 
                                //have a method for adding a state expansion to the stack.
                                m_levels[MIN_STACK_LEVEL]->add_state(
                                        new stack_state(m_params, sent_data, rm_query, lm_query,
                                        bind(&multi_stack::add_stack_state, this, _1))
                                        );
                            }

                            /**
                             * The basic destructor
                             */
                            ~multi_stack() {
                                //Dispose the stacks
                                if (m_levels != NULL) {

                                    //Iterate through the levels and delete them
                                    for (uint32_t level = MIN_STACK_LEVEL; level < m_num_levels; ++level) {
                                        delete m_levels[level];
                                    }

                                    //Delete the levels array itself
                                    delete[] m_levels;
                                    m_levels = NULL;
                                }
                            }

                            /**
                             * Allows to extend the hypothesis, when extending the stack we immediately re-combine
                             */
                            void expand() {
                                //Iterate the stack levels and expand them one by one 
                                //until the last one or until we are requested to stop
                                while (!m_is_stop && (m_curr_level < m_num_levels)) {
                                    //Here we expand the stack level and then
                                    //increment the current level index variable
                                    m_levels[m_curr_level++]->expand();
                                }
                            }

                            /**
                             * Allows to get the best translation from the
                             * stack after the decoding has finished.
                             * @param target_sent [out] the variable to store the translation
                             */
                            void get_best_trans(string & target_sent) const {
                                if (!m_is_stop) {
                                    //Sanity check that the translation has been finished
                                    ASSERT_SANITY_THROW((m_curr_level != m_num_levels),
                                            string("The translation was not finished, ") +
                                            string("the next-to-consider stack level is ") +
                                            to_string(m_curr_level) + string(" the last-to-") +
                                            string("consider is ") + to_string(m_num_levels - 1));

                                    //Request the last level for the best translation
                                    m_levels[m_num_levels - 1]->get_best_trans(target_sent);
                                }
                            }

                        protected:

                            /**
                             * Allows to add a new stack state into the proper stack level
                             * @param new_state the new stack state, not NULL
                             */
                            void add_stack_state(stack_state_ptr new_state) {
                                //Perform a NULL pointer sanity check
                                ASSERT_SANITY_THROW((new_state == NULL), "A NULL pointer stack state!");

                                //Get the new state stack level
                                const uint32_t level = new_state->get_stack_level();

                                //Perform a sanity check that the state is of a proper level
                                ASSERT_SANITY_THROW((level >= m_num_levels),
                                        string("The new stack state stack level is too big: ") +
                                        to_string(level) + string(" the maximum allowed is: ") +
                                        to_string(m_num_levels - 1));

                                //Add the state to the corresponding stack
                                m_levels[level]->add_state(new_state);
                            }

                        private:
                            //Stores the reference to the decoder parameters
                            const de_parameters & m_params;
                            //Stores the stopping flag
                            acr_bool_flag m_is_stop;

                            //The reference to the sentence data map
                            const sentence_data_map & m_sent_data;
                            //The reference to the reordering mode query data
                            const rm_query_proxy & m_rm_query;
                            //Sores the language mode query proxy
                            lm_fast_query_proxy & m_lm_query;

                            //Stores the number of multi-stack levels
                            const uint32_t m_num_levels;
                            //Stores the current stack level index
                            uint32_t m_curr_level;
                            
                            //This is a pointer to the array of stacks, one stack per number of covered words.
                            stack_level_ptr * m_levels;
                        };
                    }
                }
            }
        }
    }
}

#endif /* TRANS_STACK_HPP */

