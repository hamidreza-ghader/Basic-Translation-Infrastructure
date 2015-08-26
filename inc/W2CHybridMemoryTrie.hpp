/* 
 * File:   W2CHybridMemoryTrie.hpp
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
 * Created on August 21, 2015, 4:18 PM
 */

#ifndef W2CHYBRIDMEMORYTRIE_HPP
#define	W2CHYBRIDMEMORYTRIE_HPP

#include <string>   // std::string

#include "Globals.hpp"
#include "Logger.hpp"
#include "ATrie.hpp"
#include "AWordIndex.hpp"
#include "W2CHybridMemoryTrieStorage.hpp"

using namespace std;
using namespace uva::smt::tries::dictionary;

namespace uva {
    namespace smt {
        namespace tries {

            /**
             * This is the hybrid memory trie implementation class. It has three template parameters.
             * @param N the maximum number of levelns in the trie.
             * @param StorageFactory the factory to create storage containers
             * @param StorageContainer the storage container type that is created by the factory
             */
            template<TModelLevel N, template<TModelLevel > class StorageFactory, class StorageContainer>
            class W2CHybridMemoryTrie : public ATrie<N> {
            public:

                /**
                 * The basic constructor
                 * @param p_word_index the word index (dictionary) container
                 */
                explicit W2CHybridMemoryTrie(AWordIndex * const p_word_index);

                /**
                 * This method can be used to provide the N-gram count information
                 * That should allow for pre-allocation of the memory
                 * For more details @see ATrie
                 */
                virtual void preAllocate(const size_t counts[N]);

                /**
                 * The basic destructor
                 */
                virtual ~W2CHybridMemoryTrie();

            protected:

                /**
                 * Allows to retrieve the data storage structure for the One gram with the given Id.
                 * If the storage structure does not exist, return a new one.
                 * For more details @see ATrie
                 */
                virtual TProbBackOffEntryPair & make_1_GramDataRef(const TShortId wordId) {
                    //Get the word probability and back-off data reference
                    return m_mgram_data[0][wordId];
                };

                /**
                 * Allows to retrieve the data storage structure for the One gram with the given Id.
                 * If the storage structure does not exist, throws an exception.
                 * For more details @see ATrie
                 */
                virtual const TProbBackOffEntryPair & get_1_GramDataRef(const TShortId wordId) {
                    //Get the word probability and back-off data reference
                    return m_mgram_data[0][wordId];
                };

                /**
                 * Allows to retrieve the data storage structure for the M gram
                 * with the given M-gram level Id. M-gram context and last word Id.
                 * If the storage structure does not exist, return a new one.
                 * For more details @see ATrie
                 */
                virtual TProbBackOffEntryPair& make_M_GramDataRef(const TModelLevel level, const TShortId wordId, const TLongId ctxId) {
                    //Get the word mapping first
                    StorageContainer*& ctx_mapping = m_mgram_mapping[level - MGRAM_IDX_OFFSET][wordId];

                    //If the mappins is not there yet for the contexts then create it
                    if (ctx_mapping == NULL) {
                        ctx_mapping = m_storage_factory->create(level);
                        LOG_DEBUG3 << "A new ACtxToPBStorage container is allocated for level " << SSTR(level) << END_LOG;
                    }

                    //Add the new element to the context mapping
                    TShortId & nextCtxId = ctx_mapping->operator[](ctxId);
                    nextCtxId = next_ctx_id[level - MGRAM_IDX_OFFSET]++;

                    //Return the reference to it
                    return m_mgram_data[level - 1][nextCtxId];
                };

                /**
                 * Allows to retrieve the data storage structure for the M gram
                 * with the given M-gram level Id. M-gram context and last word Id.
                 * If the storage structure does not exist, throws an exception.
                 * For more details @see ATrie
                 */
                virtual const TProbBackOffEntryPair& get_M_GramDataRef(const TModelLevel level, const TShortId wordId, const TLongId ctxId) {
                    //Try to find the word mapping first
                    StorageContainer*& ctx_mapping = m_mgram_mapping[level - MGRAM_IDX_OFFSET][wordId];

                    //If the mapping is present the search further, otherwise an exception
                    if (ctx_mapping != NULL) {
                        //Get the key context id end then obtain the data object reference
                        const TShortId & keyCtxId = ctx_mapping->at(ctxId);
                        return m_mgram_data[level - 1][keyCtxId];
                    } else {
                        LOG_DEBUG1 << "There are no elements @ level: " << SSTR(level) << " for wordId: " << SSTR(wordId) << "!" << END_LOG;
                        throw out_of_range("not found");
                    }
                };

                /**
                 * Allows to retrieve the data storage structure for the N gram.
                 * Given the N-gram context and last word Id.
                 * If the storage structure does not exist, return a new one.
                 * For more details @see ATrie
                 */
                virtual TLogProbBackOff& make_N_GramDataRef(const TShortId wordId, const TLongId ctxId) {
                    StorageContainer*& ctx_mapping = m_mgram_mapping[N - MGRAM_IDX_OFFSET][wordId];
                    if (ctx_mapping == NULL) {
                        ctx_mapping = m_storage_factory->create(N);
                        LOG_DEBUG3 << "A new ACtxToPBStorage container is allocated for level " << SSTR(N) << END_LOG;
                    }
                    return (TLogProbBackOff &) ctx_mapping->operator[](ctxId);
                };

                /**
                 * Allows to retrieve the data storage structure for the N gram.
                 * Given the N-gram context and last word Id.
                 * If the storage structure does not exist, throws an exception.
                 * For more details @see ATrie
                 */
                virtual const TLogProbBackOff& get_N_GramDataRef(const TShortId wordId, const TLongId ctxId) {
                    //Try to find the word mapping first
                    StorageContainer*& ctx_mapping = m_mgram_mapping[N - MGRAM_IDX_OFFSET][wordId];

                    //If the mapping is present the search further, otherwise an exception
                    if (ctx_mapping != NULL) {
                        return (TLogProbBackOff &) ctx_mapping->at(ctxId);
                    } else {
                        LOG_DEBUG1 << "There are no elements @ level: " << SSTR(N) << " for wordId: " << SSTR(wordId) << "!" << END_LOG;
                        throw out_of_range("not found");
                    }
                };

            private:
                //The offset, relative to the M-gram level M for the mgram mapping array index
                const static TModelLevel MGRAM_IDX_OFFSET = 2;

                //Stores the number of words
                size_t m_word_arr_size;

                //The factory to produce the storage containers
                StorageFactory<N> * m_storage_factory;

                //M-Gram data for 1 <= M < N. This is a 2D array storing
                //For each M-Gram level M an array of prob-back_off values
                // m_mgram_data[M][0] - probability/back-off pair for the given M-gram
                // m_mgram_data[M][1] --//--
                // ...
                // m_mgram_data[M][#M-Grams - 1] --//--
                // m_mgram_data[M][#M-Grams] --//--
                TProbBackOffEntryPair * m_mgram_data[N - 1];

                //M-Gram data for 1 < M <= N. This is a 2D array storing
                //For each M-Gram level M an array of #words elements of
                //pointers to C template parameter type:
                // m_mgram_mapping[M][0] -> NULL (if there is no M-gram ending with word 0 in the level)
                // m_mgram_mappin[M]g[1] -> C instance
                // ...
                // m_mgram_mapping[M][#words - 1] -> NULL
                // m_mgram_mapping[M][#words] -> C instance
                //
                //For all 1 < M < N instances of C contain mappings from the
                //context index to the index in the m_mgram_data[M] array -
                //the array storing probability/back-off values.
                //For M = N the pair value stored in the instance of C is the
                //probability itself! Note that that the probabilities are
                //stored as floats - 4 bytes and m_mgram_data[M] array is also a
                //4 byte integer, so we minimize memory usage by storing float
                //probability in place of the index.
                StorageContainer** m_mgram_mapping[N - 1];

                //Will store the next context index counters per M-gram level
                //for 1 < M < N.
                const static TModelLevel NUM_IDX_COUNTERS = N - 2;
                TShortId next_ctx_id[NUM_IDX_COUNTERS];

                /**
                 * Computes the N-Gram context using the previous context and the current word id
                 * 
                 * WARNING: Must only be called for the M-gram level 1 < M <= N!
                 * 
                 * @param wordId the current word id
                 * @param ctxId the previous context id
                 * @param level the M-gram level we are working with M
                 * @return the resulting context
                 */
                inline TLongId getContextId(TShortId wordId, TLongId ctxId, const TModelLevel level) {
                    LOG_DEBUG3 << "Retrieving context level: " << level << ", wordId: "
                            << wordId << ", ctxId: " << ctxId << END_LOG;
                    StorageContainer* ctx_mapping = m_mgram_mapping[level - MGRAM_IDX_OFFSET][wordId];
                    
                    //Check that the context data is available
                    if ( ctx_mapping != NULL) {
                        return ctx_mapping->at(ctxId);
                    } else {
                        LOG_DEBUG1 << "Can not compute context for level: " << level << ", wordId: "
                            << wordId << ", previous ctxId: " << ctxId << END_LOG;
                        throw out_of_range("not found");
                    }
                }
            };

            typedef W2CHybridMemoryTrie<MAX_NGRAM_LEVEL, CtxToPBUnorderedMapStorageFactory, CtxToPBUnorderedMapStorage> TFiveMapW2CHybridMemoryTrie;
        }
    }
}

#endif	/* HYBRIDMEMORYTRIE_HPP */
