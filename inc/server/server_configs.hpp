/* 
 * File:   server_configs.hpp
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
 * Created on February 23, 2016, 4:52 PM
 */

#ifndef SERVER_CONFIGS_HPP
#define SERVER_CONFIGS_HPP

#include <string>

using namespace std;

namespace uva {
    namespace smt {
        namespace bpbd {
            namespace server {

                //This typedef if used for the translation model level
                typedef uint16_t phrase_length;

                //The type used for storing log probabilities, back-off, and feature values
                typedef float prob_weight;

                //Declare the phrase unique identifier type
                typedef uint64_t phrase_uid;

                //Declare the word unique identifier type
                typedef uint64_t word_uid;

                //The base of the logarithm for stored probabilities/back-off weights
                static constexpr prob_weight LOG_PROB_WEIGHT_BASE = 10.0;
                //Stores the zero log probability for the partial score or future cost
                static constexpr prob_weight UNKNOWN_LOG_PROB_WEIGHT = -1000.0;
                //The zero like value for log probability weight
                static constexpr prob_weight ZERO_LOG_PROB_WEIGHT = -100.0f;

                namespace decoder {
                }

                namespace tm {
                    //Define the feature weights delimiter string for the config file
                    static const string TM_FEATURE_WEIGHTS_DELIMITER_STR = u8"|";
                    
                    //Stores the different values of TM features                    
                    static constexpr size_t FOUR_TM_FEATURES = 4u;
                    //Stores the number of the translation model features
                    static constexpr size_t NUM_TM_FEATURES = FOUR_TM_FEATURES;

                    //The considered maximum length of the target phrase
                    static constexpr uint16_t TM_MAX_TARGET_PHRASE_LEN = 7u;

                    //Stores the unknown source phrase string, should be configurable
                    static const string TM_UNKNOWN_SOURCE_STR = u8"UNK";
                    //Stores the unknown target phrase string, should be configurable
                    static const string TM_UNKNOWN_TARGET_STR = u8"<unk>";
                }

                namespace lm {
                    //Define the feature weights delimiter string for the config file
                    static const string LM_FEATURE_WEIGHTS_DELIMITER_STR = "|";

                    //Stores the different values of RM features                    
                    static constexpr size_t ONE_LM_FEATURE = 1u;
                    //Stores the number of the language model features
                    static constexpr size_t NUM_LM_FEATURES = ONE_LM_FEATURE;

                    //The considered maximum length of the N-gram 
                    static constexpr uint16_t LM_M_GRAM_LEVEL_MAX = 5u;
                    //The maximum considered length of the m-gram history
                    static constexpr uint16_t LM_HISTORY_LEN_MAX = LM_M_GRAM_LEVEL_MAX - 1;
                    //The considered maximum length of the N-gram query
                    static constexpr uint16_t LM_MAX_QUERY_LEN = tm::TM_MAX_TARGET_PHRASE_LEN + LM_HISTORY_LEN_MAX;

                    //Stores the unknown word string, should be configurable
                    static const string UNKNOWN_WORD_STR = u8"<unk>";

                    //Stores the start of the sentence symbol
                    static const string BEGIN_SENTENCE_TAG_STR = u8"<s>";
                    //Stores the end of the sentence symbol
                    static const string END_SENTENCE_TAG_STR = u8"</s>";

                    //The default value of the unknown word probability weight
                    const prob_weight DEF_UNK_WORD_LOG_PROB_WEIGHT = -10.0f;
                }

                namespace rm {
                    //Define the feature weights delimiter string for the config file
                    static const string RM_FEATURE_WEIGHTS_DELIMITER_STR = u8"|";

                    //Stores the different values of RM features  
                    static constexpr size_t TWO_RM_FEATURES = 2u;
                    static constexpr size_t FOUR_RM_FEATURES = 4u;
                    static constexpr size_t SIX_RM_FEATURES = 6u;
                    static constexpr size_t EIGHT_RM_FEATURES = 8u;
                    //Stores the number of the reordering model features
                    static constexpr size_t NUM_RM_FEATURES = SIX_RM_FEATURES;

                    //Stores the unknown source phrase string, should be configurable
                    static const string RM_UNK_SOURCE_PHRASE = u8"UNK";
                    //Stores the unknown target phrase string, should be configurable
                    static const string RM_UNK_TARGET_PHRASE = u8"UNK";
                }
            }
        }
    }
}

//INclude server constants as they depend on server configs
#include "server/server_consts.hpp"

#endif /* SERVER_CONFIGS_HPP */

