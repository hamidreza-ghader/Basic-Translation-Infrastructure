/* 
 * File:   Logger.hpp
 * Author: Dr. Ivan S. Zapreev
 *
 * Some of the ideas and code implemented here were taken from:
 *  http://www.drdobbs.com/cpp/logging-in-c/201804215?pgno=1
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
 * Created on July 26, 2015, 4:08 PM
 */

#include <algorithm>    // std::transform

#include "common/utils/logging/logger.hpp"
#include "common/utils/monitor/statistics_monitor.hpp"

using namespace uva::utils::monitor;

namespace uva {
    namespace utils {
        namespace logging {

            recursive_mutex logging_synch::mv;
            
            std::ostream& operator<<(std::ostream& stream, const unsigned char & value) {
                return stream << ((uint32_t) value);
            }

            std::ostream& operator<<(std::ostream& stream, const signed char & value) {
                return stream << ((int32_t) value);
            }

            debug_levels_enum logger::m_curr_level = debug_levels_enum::RESULT;

            const char * logger::m_debug_level_str[debug_levels_enum::size] = {
                ERROR_PARAM_VALUE, WARNING_PARAM_VALUE, USAGE_PARAM_VALUE, RESULT_PARAM_VALUE,
                INFO_PARAM_VALUE, INFO1_PARAM_VALUE, INFO2_PARAM_VALUE, INFO3_PARAM_VALUE,
                DEBUG_PARAM_VALUE, DEBUG1_PARAM_VALUE, DEBUG2_PARAM_VALUE, DEBUG3_PARAM_VALUE,
                DEBUG4_PARAM_VALUE
            };

            //Initialize the progress bar chars array
            const vector<string> logger::m_progress_chars({"///", "---", "\\\\\\", "|||", "\r\r\r"});

            //It is the number of characters minus one as the last one is backspace
            const unsigned short int logger::m_num_prog_chars = m_progress_chars.size() - 1;

            //Set the initial index to zerro
            unsigned short int logger::m_curr_prog_char_idx = 0;

            //Set the initial update time to zero
            unsigned int logger::m_update_counter = 0.0;

            //The progress bar is not running first
            bool logger::m_is_pb_on = false;

            //The action message to display
            string logger::m_prefix = "";

            //Initialize the progress bar begin time
            clock_t logger::m_begin_time = 0;
            //Initialize the previous output time string length
            size_t logger::m_time_str_len = 0;

            /**
             * Allows to retrieve the list of supporter logging levels
             * @param p_reporting_levels the pointer to the logging levels vector to be filled in
             */
            void logger::get_reporting_levels(vector<string> * p_reporting_levels) {
                for (size_t level_id = debug_levels_enum::ERROR; level_id <= MAXIMUM_LOGGING_LEVEL; ++level_id) {
                    string level = m_debug_level_str[level_id];
                    transform(level.begin(), level.end(), level.begin(), ::tolower);
                    p_reporting_levels->push_back(level);
                }
            }

            /**
             * \todo {This function is ugly improve it by using a map, or a
             * similar so that we could just get an appropriate level
             * for the string.}
             */
            void logger::set_reporting_level(string level) {
                bool isGoodLevel = true;
                debug_levels_enum new_debug_level = debug_levels_enum::USAGE;
                transform(level.begin(), level.end(), level.begin(), ::toupper);

                if (!level.compare(USAGE_PARAM_VALUE)) {
                    new_debug_level = debug_levels_enum::USAGE;
                } else {
                    if (!level.compare(RESULT_PARAM_VALUE)) {
                        new_debug_level = debug_levels_enum::RESULT;
                    } else {
                        if (!level.compare(WARNING_PARAM_VALUE)) {
                            new_debug_level = debug_levels_enum::WARNING;
                        } else {
                            if (!level.compare(INFO_PARAM_VALUE)) {
                                new_debug_level = debug_levels_enum::INFO;
                            } else {
                                if (!level.compare(INFO1_PARAM_VALUE)) {
                                    new_debug_level = debug_levels_enum::INFO1;
                                } else {
                                    if (!level.compare(INFO2_PARAM_VALUE)) {
                                        new_debug_level = debug_levels_enum::INFO2;
                                    } else {
                                        if (!level.compare(INFO3_PARAM_VALUE)) {
                                            new_debug_level = debug_levels_enum::INFO3;
                                        } else {
                                            if (!level.compare(DEBUG_PARAM_VALUE)) {
                                                new_debug_level = debug_levels_enum::DEBUG;
                                            } else {
                                                if (!level.compare(DEBUG1_PARAM_VALUE)) {
                                                    new_debug_level = debug_levels_enum::DEBUG1;
                                                } else {
                                                    if (!level.compare(DEBUG2_PARAM_VALUE)) {
                                                        new_debug_level = debug_levels_enum::DEBUG2;
                                                    } else {
                                                        if (!level.compare(DEBUG3_PARAM_VALUE)) {
                                                            new_debug_level = debug_levels_enum::DEBUG3;
                                                        } else {
                                                            if (!level.compare(DEBUG4_PARAM_VALUE)) {
                                                                new_debug_level = debug_levels_enum::DEBUG4;
                                                            } else {
                                                                isGoodLevel = false;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (isGoodLevel) {
                    if( new_debug_level <= MAXIMUM_LOGGING_LEVEL  ) {
                        //Set the new level
                        m_curr_level = new_debug_level;
                        //Log the usage info
                        LOG_USAGE << "The requested debug level is: \'" << level
                                << "\', the maximum build level is '"
                                << m_debug_level_str[MAXIMUM_LOGGING_LEVEL] << "'"
                                << " the set level is '" << m_debug_level_str[m_curr_level]
                                << "'" << END_LOG;
                    } else {
                        LOG_WARNING << "The requested debug level: '" << level
                                << "' is higher than the maximum build level: "
                                << m_debug_level_str[MAXIMUM_LOGGING_LEVEL] << ", Ignoring!" << END_LOG;
                    }
                } else {
                    LOG_WARNING << "Ignoring an unsupported value of [debug-level] parameter: '" << level << "'" << END_LOG;
                }
            }

            string logger::compute_time_string(const clock_t elapsedClockTime, size_t & timeStrLen) {
                const float timeSec = (((float) elapsedClockTime) / CLOCKS_PER_SEC);
                const uint minute = (((uint) timeSec) % 3600) / 60;
                const uint hour = ((uint) timeSec) / 3600;
                const float second = (float) (((uint) ((timeSec - minute * 60 - hour * 3600)* 100)) / 100);
                stringstream msg;
                msg << " " << SSTR(hour) << " hour(s) " << SSTR(minute) << " minute(s) " << SSTR(second) << " second(s) ";
                string result = m_prefix + msg.str();
                timeStrLen = result.size();
                return result;
            }

            string logger::compute_time_clear_string(const size_t length) {
                string result = "";
                for (size_t i = 0; i < length; i++) {
                    result += "\r";
                }
                return result;
            }

            //This macro is used to check if we need to do the progress indications
#define IS_ENOUGH_LOGGING_LEVEL(level) (( PROGRESS_ACTIVE_LEVEL <= MAXIMUM_LOGGING_LEVEL ) && ( PROGRESS_ACTIVE_LEVEL <= level ))

            void logger::start_progress_bar(const string & msg) {
                if (IS_ENOUGH_LOGGING_LEVEL(m_curr_level)) {
                    if (!m_is_pb_on) {
                        stringstream pref;
                        pref << m_debug_level_str[PROGRESS_ACTIVE_LEVEL] << ":"
                                << WHITE_SPACE_SEPARATOR << msg << ":" << WHITE_SPACE_SEPARATOR;
                        m_prefix = pref.str();

                        //Output the time string
                        cout << compute_time_string(m_begin_time, m_time_str_len);
                        cout.flush();

                        //Store the current time
                        m_begin_time = clock();

                        //Update the update counter and set the progress bar on flag
                        m_update_counter = 0;
                        m_is_pb_on = true;
                    }
                } else {
                    LOG_INFO << msg << END_LOG;
                }
            }

            void logger::update_progress_bar() {
                if (IS_ENOUGH_LOGGING_LEVEL(m_curr_level)) {
                    //Do not update each time to save on computations
                    if (m_update_counter > (CLOCKS_PER_SEC / 4)) {

                        //Output the current time
                        cout << compute_time_clear_string(m_time_str_len) << compute_time_string(clock() - m_begin_time, m_time_str_len);
                        cout.flush();

                        m_update_counter = 0;
                    } else {

                        m_update_counter++;
                    }
                }
            }

            void logger::stop_progress_bar() {
                if (IS_ENOUGH_LOGGING_LEVEL(m_curr_level) && m_is_pb_on) {
                    //Clear the progress
                    cout << compute_time_clear_string(m_time_str_len) << "\n";
                    cout.flush();

                    //Reset class variables
                    m_prefix = "";
                    m_begin_time = 0;
                    m_time_str_len = 0;
                    m_update_counter = 0;
                    m_is_pb_on = false;
                }
            }
        }
    }
}