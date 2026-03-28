/*
 * Copyright (c) 2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * Describes a eip prefetcher based on template policies.
 */

#include "mem/cache/prefetch/eip.hh"

#include <algorithm>

#include "params/EIPPrefetcher.hh"

#define DEGREE 2  // Doesn't matter for EIP

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

EIP::EIP(const EIPPrefetcherParams &p)
    : Queued(p)
{
    l1i_last_basic_block = 0;
    l1i_consecutive_count = 0;
    l1i_basic_block_merge_diff = 0;

    l1i_init_hist_table();
    l1i_init_timing_tables();
    l1i_init_entangled_table();
}

void
EIP::add_pfmq(uint64_t lineAddr, uint32_t sourceSet, uint32_t sourceWay)
{
    pfmq.push_back({lineAddr,sourceSet,sourceWay});
}

void
EIP::remove_pfmq(uint64_t lineAddr)
{
    auto it = std::find_if(pfmq.begin(),pfmq.end(),
            [lineAddr](struct pfInMSHRQ pfm){
        return lineAddr==pfm.lineAddr;
    });
    if (it != pfmq.end())
    {
        pfmq.erase(it);
    }
}

void
EIP::clear_pfmq(){
    pfmq.clear();
}

uint64_t
EIP::getSourceSet(uint64_t lineAddr){
    auto it = std::find_if(
        pfmq.begin(),
        pfmq.end(),
        [lineAddr](struct pfInMSHRQ pfm){
            return lineAddr==pfm.lineAddr;
        }
    );
    if (it != pfmq.end())
    {
        return it->sourceSet;
    }
    return 0;
}
uint64_t
EIP::getSourceWay(uint64_t lineAddr)
{
    auto it = std::find_if(
        pfmq.begin(),
        pfmq.end(),
        [lineAddr](struct pfInMSHRQ pfm){
            return lineAddr==pfm.lineAddr;
        }
    );
    if (it != pfmq.end())
    {
        return it->sourceWay;
    }
    return 0;
}

void
EIP::prefetch_issued_to_pq(uint64_t lineAddr,
    uint64_t sourceSet, uint64_t sourceWay)
{
    l1i_add_timing_entry(lineAddr,sourceSet,sourceWay);
}

void
EIP::calculatePrefetch(const PrefetchInfo &pfi,
                        std::vector<AddrPriority> &addresses)
{

    uint64_t instr_id = pfi.getInstrId();

    uint64_t addr = pfi.getAddr();

    uint64_t line_addr = addr >> LOG2_BLOCK_SIZE;
    bool cache_hit = !(pfi.isCacheMiss());
    bool prefetch_hit = pfi.isPrefetchHit();

    if (cache_hit && l1i_find_timing_cache_entry(line_addr) == MAX_NUM_WAY ){
        std::cout<< "[Fail] Cache Hit but not in Timing Cache: Addr "
            <<line_addr<<" with set "<<line_addr%64<<std::endl;
    }
    if (!cache_hit && l1i_find_timing_cache_entry(line_addr) < MAX_NUM_WAY ){
        std::cout<< "[Fail] Cache Miss but hit in Timing Cache: Addr "
            <<line_addr<<" with set "<<line_addr%64<<std::endl;
    }
    if (!cache_hit)
        assert(l1i_find_timing_cache_entry(line_addr) == MAX_NUM_WAY);
    if (cache_hit)
        assert(l1i_find_timing_cache_entry(line_addr) < MAX_NUM_WAY);

    bool consecutive = false;

    if (l1i_last_basic_block + l1i_consecutive_count == line_addr)
    { // Same
        return;
    }
    else if (l1i_last_basic_block + l1i_consecutive_count + 1 == line_addr)
    { // Consecutive
        l1i_consecutive_count++;
        consecutive = true;
    }

    uint32_t format = L1I_ENTANGLED_NUM_FORMATS;
    uint32_t num_entangled = 0;

    //@Alberto: Optimization: Only prefetch if cache miss or prefetch hit
    // if (!cache_hit || prefetch_hit){

    // Queue basic block prefetches
    uint32_t bb_size = l1i_get_bbsize_entangled_table(line_addr);
    for (uint32_t i = 1; i <= bb_size; i++)
    {
        uint64_t pf_addr = addr + i * (1 << LOG2_BLOCK_SIZE);
        // Issue prefetches
        if (!l1i_ongoing_request(pf_addr >> LOG2_BLOCK_SIZE))
        {
            addresses.push_back(AddrPriority(pf_addr, 0));
            add_pfmq(pf_addr, 0, L1I_ET_WAYS);
        }
    }

    for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
    {
        uint32_t source_set = 0;
        uint32_t source_way = L1I_ET_WAYS;
        uint64_t entangled_line_addr = l1i_get_entangled_addr_entangled_table(
                line_addr, k,
                source_set,
                source_way
            );
        if (entangled_line_addr && (entangled_line_addr != line_addr))
        {
            uint32_t format_k =
                l1i_get_format_entangled(line_addr, entangled_line_addr);
            if (format_k < format)
                format = format_k;
            num_entangled++;
            bb_size = l1i_get_bbsize_entangled_table(entangled_line_addr);
            for (uint32_t i = 0; i <= bb_size; i++)
            {
                uint64_t pf_line_addr = entangled_line_addr + i;
                // Decrease reduntant entangled destinations
                assert(source_way < L1I_ET_WAYS);
                if (l1i_different_timing_entry_src(
                    pf_line_addr,
                    source_set,
                    source_way
                ))
                {
                    l1i_update_confidence_entangled_table(source_set,
                        source_way, pf_line_addr, false);
                }
                // Issue prefetches
                if (!l1i_ongoing_request(pf_line_addr))
                {
                    addresses.push_back(AddrPriority(
                        pf_line_addr <<
                        LOG2_BLOCK_SIZE, 0
                    ));
                    add_pfmq(pf_line_addr << LOG2_BLOCK_SIZE,
                        source_set, (i == 0) ? source_way : L1I_ET_WAYS);
                }
            }
        }
    }
    assert(format >= num_entangled);
    if (num_entangled == 0 && bb_size == 0)
        format = 0; // Fully-invalid/not-present entry

    // }

    if (!consecutive)
    { // New basic block found
        uint32_t max_bb_size =
            l1i_get_bbsize_entangled_table(l1i_last_basic_block);

        // Check for merging bb opportunities
        if (l1i_consecutive_count)
        {
            if (l1i_basic_block_merge_diff > 0)
            {
                l1i_add_bb_size_hist_table(
                    l1i_last_basic_block - l1i_basic_block_merge_diff,
                    l1i_consecutive_count + l1i_basic_block_merge_diff
                );
            }
            else
            {
                l1i_add_bb_size_hist_table(
                    l1i_last_basic_block,
                    std::max(max_bb_size, l1i_consecutive_count)
                );
            }
        }
    }

    if (!consecutive)
    { // New basic block found
        l1i_consecutive_count = 0;
        l1i_last_basic_block = line_addr;
    }

    if (!consecutive)
    {
        l1i_basic_block_merge_diff =
            l1i_find_bb_merge_hist_table(l1i_last_basic_block);
    }

    // Add the request in the history buffer
    uint32_t pos_hist = L1I_HIST_TABLE_ENTRIES;
    if (!consecutive && l1i_basic_block_merge_diff == 0)
    {
        if ((l1i_find_hist_entry(line_addr) == L1I_HIST_TABLE_ENTRIES))
        {
            pos_hist = l1i_add_hist_table(
                line_addr,
                instr_id,
                format,
                num_entangled
            );
            // false
        }
        else
        {
            if (!cache_hit && !l1i_ongoing_accessed_request(line_addr))
            {
                pos_hist = l1i_add_hist_table(
                    line_addr,
                    instr_id,
                    format,
                    num_entangled
                );
                // false
            }
        }
    }

    // Add miss in the latency table
    if (!cache_hit && !l1i_ongoing_request(line_addr))
    {
        l1i_add_timing_entry(line_addr, 0, L1I_ET_WAYS);
    }
    l1i_access_timing_entry(line_addr, pos_hist);

    return;
}

void
EIP::notifySquash(const PacketPtr &pkt)
{
    // Prefetcher squash
    uint64_t ip = pkt->req->getPC();
    uint64_t instr_id = pkt->instr_id;
    uint64_t line_addr = ip >> LOG2_BLOCK_SIZE;

    l1i_squash_hist_table(instr_id, line_addr);
    uint32_t last = (l1i_hist_table_head + L1I_HIST_TABLE_MASK)
            % L1I_HIST_TABLE_ENTRIES;
    if ((l1i_hist_table[last].tag <= line_addr &&
         (l1i_hist_table[last].tag +
         l1i_basic_block_merge_diff +
         l1i_consecutive_count)
         >= line_addr) ||
        (l1i_hist_table[last].tag <= line_addr &&
            (l1i_hist_table[last].tag + l1i_hist_table[last].bb_size)
            >= line_addr
        )
    )
    {
        l1i_hist_table[last].bb_size = 0;
        l1i_last_basic_block = l1i_hist_table[last].tag;
        l1i_consecutive_count = line_addr - l1i_last_basic_block;
    }
    else
    {
        l1i_last_basic_block = 0;
        l1i_consecutive_count = 0;
    }
    l1i_basic_block_merge_diff = 0;
}

void
EIP::notifyFill(const PacketPtr &pkt)
{
    uint64_t evicted_addr = pkt->getEvictedAddr();
    uint64_t addr = pkt->req->getVaddr();

    uint64_t line_addr = (addr >> LOG2_BLOCK_SIZE);
    uint64_t evicted_line_addr = (evicted_addr >> LOG2_BLOCK_SIZE);

    uint32_t pos_hist = L1I_HIST_TABLE_ENTRIES;
    uint64_t latency = l1i_get_latency_timing_mshr(line_addr, pos_hist);

    l1i_move_timing_entry(line_addr);

    // Get and update entangled
    if (latency && pos_hist < L1I_HIST_TABLE_ENTRIES)
    {
        uint64_t src_entangled = l1i_get_src_entangled_hist_table(
            line_addr, pos_hist,
          latency
        );
        if (src_entangled)
        {
            assert(line_addr != src_entangled);
            l1i_hist_table[pos_hist].ent_src = src_entangled;
        }
    }

    return;
}

void
EIP::notifyInvalidate(const Addr &addr){
    uint64_t line_addr = addr>>LOG2_BLOCK_SIZE;
    uint32_t source_set = 0;
    uint32_t source_way = L1I_ET_WAYS;
    uint32_t set = line_addr % MAX_NUM_SET;
    uint32_t way = l1i_find_timing_cache_entry(line_addr);
    if (way != MAX_NUM_WAY){
        l1i_timing_cache_table[set][way].valid = false;
        source_set = l1i_timing_cache_table[set][way].source_set;
        source_way = l1i_timing_cache_table[set][way].source_way;
        if (source_way < L1I_ET_WAYS)
        {
            // If accessed hit, but if not wrong
            l1i_update_confidence_entangled_table(
                source_set,
                source_way,
                line_addr,
                l1i_timing_cache_table[set][way].accessed
            );
        }
    }
}

} // namespace prefetch
} // namespace gem5
