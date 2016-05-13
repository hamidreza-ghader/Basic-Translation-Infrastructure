/* 
 * File:   server_constants.hpp
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
 * Created on February 23, 2016, 5:09 PM
 */

#include <string>

#ifndef SERVER_CONSTANTS_HPP
#define SERVER_CONSTANTS_HPP

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

                //Stores the word hash for an undefined phrase, is 0
                //WARNING! MUST BE 0 as this is the value of a default initialized integer!
                static constexpr phrase_uid UNDEFINED_PHRASE_ID = 0;
                //Define the unknown phrase id value
                static constexpr phrase_uid UNKNOWN_PHRASE_ID = UNDEFINED_PHRASE_ID + 1;
                //Contains the minimum valid phrase id value
                static constexpr phrase_uid MIN_VALID_PHRASE_ID = UNKNOWN_PHRASE_ID + 1;
                //Contains the maximum valid phrase id value
                static constexpr phrase_uid MAX_VALID_PHRASE_ID = UINT64_MAX;

                //Stores the word hash for an undefined word, is 0
                //WARNING! MUST BE 0 as this is the value of a default initialized integer!
                static constexpr word_uid UNDEFINED_WORD_ID = 0;
                //Stores the word id for an unknown word, it must have value 1
                static constexpr word_uid UNKNOWN_WORD_ID = (UNDEFINED_WORD_ID + 1);
                //Stores the minimum known word id, it must have value 2
                static constexpr word_uid MIN_KNOWN_WORD_ID = (UNKNOWN_WORD_ID + 1);
                //The word indexes that start from 2, as 0 is given to UNDEFINED and 1 to UNKNOWN (<unk>)
                static constexpr word_uid EXTRA_NUMBER_OF_WORD_IDs = 2;

                //Various M-gram levels
                const static phrase_length M_GRAM_LEVEL_UNDEF = 0u; //MUST BE ZERO
                const static phrase_length M_GRAM_LEVEL_1 = 1u;
                const static phrase_length M_GRAM_LEVEL_2 = 2u;
                const static phrase_length M_GRAM_LEVEL_3 = 3u;
                const static phrase_length M_GRAM_LEVEL_4 = 4u;
                const static phrase_length M_GRAM_LEVEL_5 = 5u;
                const static phrase_length M_GRAM_LEVEL_6 = 6u;
                const static phrase_length M_GRAM_LEVEL_7 = 7u;
                const static phrase_length M_GRAM_LEVEL_8 = 8u;
                const static phrase_length M_GRAM_LEVEL_9 = 9u;
                const static phrase_length M_GRAM_LEVEL_10 = 10u;
                const static phrase_length M_GRAM_LEVEL_11 = 11u;
                const static phrase_length M_GRAM_LEVEL_12 = 12u;

                namespace decoder {
                    //The number of best translations parameter name
                    static const string DE_NUM_BEST_TRANS_PARAM_NAME = "de_num_best_trans";
                    //The pruning threshold parameter name
                    static const string DE_PRUNING_THRESHOLD_PARAM_NAME = "de_pruning_threshold";
                    //The stack level capacity parameter name
                    static const string DE_STACK_CAPACITY_PARAM_NAME = "de_stack_capacity";
                    //The maximum source phrase length parameter name
                    static const string DE_MAX_SP_LEN_PARAM_NAME = "de_max_source_phrase_length";
                    //The maximum target phrase length parameter name
                    static const string DE_MAX_TP_LEN_PARAM_NAME = "de_max_target_phrase_length";
                    //The is-generate-search-lattice parameter name
                    static const string DE_IS_GEN_LATTICE_PARAM_NAME = "de_is_gen_lattice";
                }
                
                namespace tm {
                    //Define the feature weights delimiter string for the config file
                    static const string TM_FEATURE_WEIGHTS_DELIMITER_STR = u8"|";
                    
                    //Stores the different values of TM features
                    static constexpr size_t ONE_TM_FEATURES = 4u;
                    static constexpr size_t TWO_TM_FEATURES = 4u;
                    static constexpr size_t THREE_TM_FEATURES = 4u;
                    static constexpr size_t FOUR_TM_FEATURES = 4u;

                    //Stores the unknown source phrase string, should be configurable
                    static const string TM_UNKNOWN_SOURCE_STR = u8"UNK";
                    //Stores the unknown target phrase string, should be configurable
                    static const string TM_UNKNOWN_TARGET_STR = u8"<unk>";

                    //The TM connection string parameter name
                    static const string TM_CONN_STRING_PARAM_NAME = "tm_conn_string";
                    //The feature weights parameter name
                    static const string TM_FEATURE_PARAM_NAME = "tm_feature_weights";
                    //The unknown translation feature weights parameter name
                    static const string TM_UNK_FEATURE_PARAM_NAME = "tm_unk_features";
                    //The word penalty weight parameter name
                    static const string TM_WORD_PENALTY_PARAM_NAME = "tm_word_penalty";
                    //The phrase penalty weight parameter name
                    static const string TM_PHRASE_PENALTY_PARAM_NAME = "tm_phrase_penalty";
                    //The translation limit parameter name
                    static const string TM_TRANS_LIM_PARAM_NAME = "tm_trans_lim";
                    //The minimum translation probability parameter name
                    static const string TM_MIN_TRANS_PROB_PARAM_NAME = "tm_min_trans_prob";
                }

                namespace lm {
                    //Define the feature weights delimiter string for the config file
                    static const string LM_FEATURE_WEIGHTS_DELIMITER_STR = "|";

                    //Stores the different values of RM features                    
                    static constexpr size_t ONE_LM_FEATURE = 1u;

                    //Stores the unknown word string, should be configurable
                    static const string UNKNOWN_WORD_STR = u8"<unk>";

                    //Stores the start of the sentence symbol
                    static const string BEGIN_SENTENCE_TAG_STR = u8"<s>";
                    //Stores the end of the sentence symbol
                    static const string END_SENTENCE_TAG_STR = u8"</s>";

                    //The LM connection string parameter name
                    static const string LM_CONN_STRING_PARAM_NAME = "lm_conn_string";
                    //The feature weights parameter name
                    static const string LM_FEATURE_PARAM_NAME = "lm_feature_weights";
                }

                namespace rm {
                    //Define the feature weights delimiter string for the config file
                    static const string RM_FEATURE_WEIGHTS_DELIMITER_STR = u8"|";

                    //Stores the unknown source phrase string, should be configurable
                    static const string RM_UNK_SOURCE_PHRASE = u8"UNK";
                    //Stores the unknown target phrase string, should be configurable
                    static const string RM_UNK_TARGET_PHRASE = u8"UNK";

                    //Stores the different values of RM features  
                    static constexpr size_t TWO_RM_FEATURES = 2u;
                    static constexpr size_t FOUR_RM_FEATURES = 4u;
                    static constexpr size_t SIX_RM_FEATURES = 6u;
                    static constexpr size_t EIGHT_RM_FEATURES = 8u;
                    
                    //The RM connection string parameter name
                    static const string RM_CONN_STRING_PARAM_NAME = "rm_conn_string";
                    //The feature weights parameter name
                    static const string RM_FEATURE_PARAM_NAME = "rm_feature_weights";
                    //The linear distortion weight parameter name
                    static const string RM_DIST_LIMIT_PARAM_NAME = "rm_dist_lim";
                    //The distortion limit parameter name
                    static const string RM_LIN_DIST_PARAM_NAME = "rm_lin_dist_penalty";
                }                
            }
        }
    }
}

#endif /* SERVER_CONSTANTS_HPP */

