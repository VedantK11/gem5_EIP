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
 * Describes a tagged prefetcher.
 */

#ifndef __MEM_CACHE_PREFETCH_TAGGED_HH__
#define __MEM_CACHE_PREFETCH_TAGGED_HH__

#include "mem/cache/prefetch/queued.hh"
#include "mem/packet.hh"

#define MAX_NUM_SET 64
#define MAX_NUM_WAY 8
#define LOG2_BLOCK_SIZE 6

#define L1I_HIST_TABLE_ENTRIES 32 // 16

// LINE AND MERGE BASIC BLOCK SIZE
#define L1I_MERGE_BBSIZE_BITS 6
#define L1I_MERGE_BBSIZE_MAX_VALUE ((1 << L1I_MERGE_BBSIZE_BITS) - 1)

// TIME AND OVERFLOWS
#define L1I_TIME_DIFF_BITS 20
#define L1I_TIME_DIFF_OVERFLOW ((uint64_t)1 << L1I_TIME_DIFF_BITS)
#define L1I_TIME_DIFF_MASK (L1I_TIME_DIFF_OVERFLOW - 1)

#define L1I_TIME_BITS 12
#define L1I_TIME_OVERFLOW ((uint64_t)1 << L1I_TIME_BITS)
#define L1I_TIME_MASK (L1I_TIME_OVERFLOW - 1)

// ENTANGLED COMPRESSION FORMAT
#define L1I_ENTANGLED_MAX_FORMATS 7

#define L1I_ENTANGLED_NUM_FORMATS 6

// uint32_t L1I_ENTANGLED_FORMATS[L1I_ENTANGLED_MAX_FORMATS] =
//              {43, 20, 13, 9, 6, 5};
// #define L1I_ENTANGLED_NUM_FORMATS 6

// uint32_t L1I_ENTANGLED_FORMATS[L1I_ENTANGLED_MAX_FORMATS] =
//               {42, 20, 12, 9};
// #define L1I_ENTANGLED_NUM_FORMATS 4

// uint32_t L1I_ENTANGLED_FORMATS[L1I_ENTANGLED_MAX_FORMATS] =
//                {43, 0, 13, 9, 7};
// #define L1I_ENTANGLED_NUM_FORMATS 5

#define L1I_ET_WAYS 16
#define L1I_ENTANGLED_TABLE_INDEX_BITS 8  // 4K entries 256x16
#define L1I_ET_SETS (1 << L1I_ENTANGLED_TABLE_INDEX_BITS)
#define L1I_MAX_ENTANGLED_PER_LINE L1I_ENTANGLED_NUM_FORMATS

// HISTORY TABLE (BUFFER)
#define L1I_HIST_TABLE_MASK (L1I_HIST_TABLE_ENTRIES - 1)
#define L1I_BB_MERGE_ENTRIES 5
#define L1I_HIST_TAG_BITS 58
#define L1I_HIST_TAG_MASK (((uint64_t)1 << L1I_HIST_TAG_BITS) - 1)
#define L1I_HIST_NUM_SEARCHES 24 // L1I_HIST_TABLE_ENTRIES

// TIMING TABLES
#define L1I_SET_BITS 6
#define L1I_TIMING_MSHR_SIZE 1024
#define L1I_TIMING_MSHR_TAG_BITS 42
#define L1I_TIMING_MSHR_TAG_MASK (((uint64_t)1 << L1I_HIST_TAG_BITS) - 1)
#define L1I_TIMING_CACHE_TAG_BITS (L1I_TIMING_MSHR_TAG_BITS - L1I_SET_BITS)
#define L1I_TIMING_CACHE_TAG_MASK (((uint64_t)1 << L1I_HIST_TAG_BITS) - 1)

// ENTANGLED TABLE
#define L1I_TAG_BITS 10
#define L1I_TAG_MASK (((uint64_t)1 << L1I_TAG_BITS) - 1)
#define L1I_CONFIDENCE_BITS 2
#define L1I_CONFIDENCE_COUNTER_MAX_VALUE ((1 << L1I_CONFIDENCE_BITS) - 1)
#define L1I_CONFIDENCE_COUNTER_THRESHOLD 1

namespace gem5
{

  struct EIPPrefetcherParams;

  GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
  namespace prefetch
  {

    class EIP : public Queued
    {
    protected:
    public:
      EIP(const EIPPrefetcherParams &p);
      ~EIP() = default;

      uint64_t l1i_current_cycle;

      uint64_t l1i_last_basic_block;
      uint32_t l1i_consecutive_count;
      uint32_t l1i_basic_block_merge_diff;

      // SIZE
      uint32_t l1i_history_table_size;
      uint32_t l1i_timing_mshr_table_size;
      uint32_t l1i_confidence_cache_table_size;
      uint32_t l1i_entangled_table_size;
      uint32_t l1i_xp_queue_size;

      uint64_t get_l1i_current_cycle()
      {
        return curTick() / 500;
      }

      uint64_t l1i_get_latency(uint64_t cycle, uint64_t cycle_prev)
      {
        uint64_t cycle_masked = cycle & L1I_TIME_MASK;
        uint64_t cycle_prev_masked = cycle_prev & L1I_TIME_MASK;
        if (cycle_prev_masked > cycle_masked)
        {
          return (cycle_masked + L1I_TIME_OVERFLOW) - cycle_prev_masked;
        }
        return cycle_masked - cycle_prev_masked;
      }

      uint32_t L1I_ENTANGLED_FORMATS[L1I_ENTANGLED_MAX_FORMATS] =
                  {58, 28, 18, 13, 10, 8, 6};

      uint32_t l1i_get_format_entangled(
        uint64_t line_addr,
        uint64_t entangled_addr)
      {
        for (uint32_t i = L1I_ENTANGLED_NUM_FORMATS; i != 0; i--)
        {
          if ((line_addr >> L1I_ENTANGLED_FORMATS[i - 1]) ==
                        (entangled_addr >> L1I_ENTANGLED_FORMATS[i - 1]))
          {
            return i;
          }
        }
        assert(false);
      }

      uint64_t l1i_extend_format_entangled(uint64_t line_addr,
                          uint64_t entangled_addr, uint32_t format)
      {
        return ((line_addr >> L1I_ENTANGLED_FORMATS[format - 1])
         << L1I_ENTANGLED_FORMATS[format - 1]) |
        (entangled_addr & (((uint64_t)1 <<
                  L1I_ENTANGLED_FORMATS[format - 1]) - 1));
      }

      uint64_t l1i_compress_format_entangled(uint64_t entangled_addr,
              uint32_t format)
      {
        return entangled_addr &
              (((uint64_t)1 << L1I_ENTANGLED_FORMATS[format - 1]) - 1);
      }

      typedef struct __l1i_entangled_entry
      {
        uint64_t tag;
        uint32_t format;
        uint64_t entangled_addr[L1I_MAX_ENTANGLED_PER_LINE];
        uint32_t entangled_conf[L1I_MAX_ENTANGLED_PER_LINE];
        uint32_t bb_size;
      } l1i_entangled_entry;

      l1i_entangled_entry
          l1i_entangled_table[L1I_ET_SETS][L1I_ET_WAYS];
      uint32_t l1i_entangled_fifo[L1I_ET_SETS];

      // HISTORY TABLE (BUFFER)
      typedef struct __l1i_hist_entry
      {
        uint64_t tag;       // L1I_HIST_TAG_BITS bits
        uint64_t time_diff; // L1I_TIME_DIFF_BITS bits
        uint32_t bb_size;   // L1I_MERGE_BBSIZE_BITS bits
        uint64_t instr_id;
        uint64_t ent_src;
        uint32_t format;  // log2(L1I_ENTANGLED_NUM_FORMATS) bits
        uint32_t num_dst; // log2(L1I_ENTANGLED_NUM_FORMATS) bits
      } l1i_hist_entry;

      l1i_hist_entry l1i_hist_table[L1I_HIST_TABLE_ENTRIES];
      uint64_t l1i_hist_table_head;      // log_2 (L1I_HIST_TABLE_ENTRIES)
      uint64_t l1i_hist_table_head_time; // 64 bits

      void l1i_init_hist_table()
      {
        l1i_hist_table_head = 0;
        l1i_current_cycle = get_l1i_current_cycle();
        l1i_hist_table_head_time = l1i_current_cycle;
        for (uint32_t i = 0; i < L1I_HIST_TABLE_ENTRIES; i++)
        {
          l1i_hist_table[i].tag = 0;
          l1i_hist_table[i].time_diff = 0;
          l1i_hist_table[i].bb_size = 0;
          l1i_hist_table[i].instr_id = 0;
          l1i_hist_table[i].ent_src = 0;
          l1i_hist_table[i].format = 0;
          l1i_hist_table[i].num_dst = 0;
        }
      }

      uint64_t l1i_find_hist_entry(uint64_t line_addr)
      {
        uint64_t tag = line_addr & L1I_HIST_TAG_MASK;
        for (uint32_t count = 0,
          i = (l1i_hist_table_head + L1I_HIST_TABLE_MASK) %
                  L1I_HIST_TABLE_ENTRIES;
          count < L1I_HIST_TABLE_ENTRIES;
          count++, i = (i + L1I_HIST_TABLE_MASK) % L1I_HIST_TABLE_ENTRIES)
        {
          if (l1i_hist_table[i].tag == tag)
            return i;
        }
        return L1I_HIST_TABLE_ENTRIES;
      }

      // add_hist moved down
      void l1i_add_bb_size_hist_table(uint64_t line_addr,
              uint32_t bb_size)
      {
        uint64_t index = l1i_find_hist_entry(line_addr);
        assert(index < L1I_HIST_TABLE_ENTRIES);
        l1i_hist_table[index].bb_size = bb_size
                  & L1I_MERGE_BBSIZE_MAX_VALUE;
      }

      void l1i_squash_hist_table(uint64_t instr_id, uint64_t line_addr)
      {
        uint32_t last = (l1i_hist_table_head + L1I_HIST_TABLE_MASK) %
                L1I_HIST_TABLE_ENTRIES;
        uint64_t time_to_add = 0;
        bool found_smaller = false;
        uint32_t new_head = last;
        for (uint32_t count = 0; count < L1I_HIST_TABLE_ENTRIES; count++)
        {
          if (l1i_hist_table[last].instr_id > instr_id)
          {
            l1i_hist_table[last].tag = 0;
            if (!found_smaller)
            {
              time_to_add += l1i_hist_table[last].time_diff;
              l1i_hist_table[last].time_diff = 0;
            }
            l1i_hist_table[last].bb_size = 0;
            l1i_hist_table[last].instr_id = 0;
            l1i_hist_table[last].ent_src = 0;
          }
          else
          {
            if (l1i_hist_table[last].tag <= line_addr &&
              l1i_hist_table[last].tag + l1i_hist_table[last].bb_size
                >= line_addr)
            { // In between
              // Fix bb size in history
              l1i_hist_table[last].bb_size =
                    line_addr - l1i_hist_table[last].tag;
            }
            if (!found_smaller)
            {
              found_smaller = true;
              l1i_hist_table[last].time_diff += time_to_add;
              new_head = last;
            }
          }
          last = (last + L1I_HIST_TABLE_MASK) % L1I_HIST_TABLE_ENTRIES;
        }
        l1i_hist_table_head = (new_head + 1) % L1I_HIST_TABLE_ENTRIES;
      }

      uint32_t l1i_find_bb_merge_hist_table(uint64_t line_addr)
      {
        uint64_t tag = line_addr & L1I_HIST_TAG_MASK;
        for (uint32_t count = 0,
          i = (l1i_hist_table_head + L1I_HIST_TABLE_MASK)
                % L1I_HIST_TABLE_ENTRIES;
          count < L1I_HIST_TABLE_ENTRIES; count++,
          i = (i + L1I_HIST_TABLE_MASK) % L1I_HIST_TABLE_ENTRIES)
        {
          if (count >= L1I_BB_MERGE_ENTRIES)
          {
            return 0;
          }
          if (tag > l1i_hist_table[i].tag &&
                 (tag - l1i_hist_table[i].tag) <=
                l1i_hist_table[i].bb_size)
          { // try: bb_size + 1
            return tag - l1i_hist_table[i].tag;
          }
        }
        assert(false);
      }

      // return src-entangled pair
      uint64_t l1i_get_src_entangled_hist_table(uint64_t line_addr,
        uint32_t pos_hist, uint64_t latency,
        uint32_t skip = 0)
      {
        assert(pos_hist < L1I_HIST_TABLE_ENTRIES);
        uint64_t tag = line_addr & L1I_HIST_TAG_MASK;
        assert(tag);
        if (l1i_hist_table[pos_hist].tag != tag)
        {
          return 0; // removed
        }
        uint32_t next_pos = (pos_hist + L1I_HIST_TABLE_MASK)
              % L1I_HIST_TABLE_ENTRIES;
        uint32_t first = (l1i_hist_table_head + L1I_HIST_TABLE_MASK)
              % L1I_HIST_TABLE_ENTRIES;
        uint64_t time_i = l1i_hist_table[pos_hist].time_diff;
        uint32_t num_searched = 0;
        uint64_t best_entangled = 0;
        uint32_t pos_best = 0;
        uint32_t format_best = 0;
        uint8_t found = 0;
        for (uint32_t count = 0, i = next_pos;
            i != first; count++,
            i = (i + L1I_HIST_TABLE_MASK) % L1I_HIST_TABLE_ENTRIES)
        {
          // Against the time overflow
          if (l1i_hist_table[i].tag == tag)
          {
            return 0;
          }
          if (l1i_hist_table[i].tag && time_i >= latency)
          {
            num_searched++;
            uint32_t format_i =
                  l1i_get_format_entangled(l1i_hist_table[i].tag, tag);
            uint32_t new_format =
            (l1i_hist_table[i].num_dst == 0 ||
              format_i < l1i_hist_table[i].format) ?
                format_i : l1i_hist_table[i].format;
            if (num_searched == 1)
            {
              best_entangled = l1i_hist_table[i].tag;
              pos_best = i;
              format_best = new_format;
            }
            if (l1i_hist_table[i].format != 0)
            { // If source present in entangled table
              if (l1i_hist_table[i].num_dst < new_format)
              { // Fits
                if (format_i == l1i_hist_table[i].format)
                { // with same format
                  best_entangled = l1i_hist_table[i].tag;
                  pos_best = i;
                  format_best = new_format;
                  break;
                }
                else if (found < 2)
                { // with smaller format
                  best_entangled = l1i_hist_table[i].tag;
                  pos_best = i;
                  format_best = new_format;
                  found = 2;
                }
              }
            }
            else if (found == 0)
            {
              best_entangled = l1i_hist_table[i].tag;
              pos_best = i;
              format_best = new_format;
              found = 1;
            }
            if (num_searched == L1I_HIST_NUM_SEARCHES)
            {
              break; // Use the first destination
            }
          }
          time_i += l1i_hist_table[i].time_diff;
        }
        if (best_entangled)
        {
          l1i_hist_table[pos_best].format = format_best;
          l1i_hist_table[pos_best].num_dst++;
          return best_entangled;
        }
        return 0;
      }

      // TIMING TABLES
      typedef struct __l1i_timing_mshr_entry
      {
        bool valid;          // 1 bit
        uint64_t tag;        // L1I_TIMING_MSHR_TAG_BITS bits
        uint32_t source_set; // 8 bits
        uint32_t source_way; // 6 bits
        uint64_t timestamp;  // L1I_TIME_BITS bits // time when issued
        bool accessed;       // 1 bit
        uint32_t pos_hist;   // 1 bit
      } l1i_timing_mshr_entry;

      typedef struct __l1i_timing_cache_entry
      {
        bool valid;          // 1 bit
        uint64_t tag;        // L1I_TIMING_CACHE_TAG_BITS bits
        uint32_t source_set; // 8 bits
        uint32_t source_way; // 6 bits
        bool accessed;       // 1 bit
      } l1i_timing_cache_entry;

      l1i_timing_mshr_entry l1i_timing_mshr_table[L1I_TIMING_MSHR_SIZE];
      l1i_timing_cache_entry l1i_timing_cache_table[MAX_NUM_SET][MAX_NUM_WAY];

      void l1i_init_timing_tables()
      {
        for (uint32_t i = 0; i < L1I_TIMING_MSHR_SIZE; i++)
        {
          l1i_timing_mshr_table[i].valid = 0;
        }
        for (uint32_t i = 0; i < MAX_NUM_SET; i++)
        {
          for (uint32_t j = 0; j < MAX_NUM_WAY; j++)
          {
            l1i_timing_cache_table[i][j].valid = 0;
          }
        }
      }

      uint64_t l1i_find_timing_mshr_entry(uint64_t line_addr)
      {
        for (uint32_t i = 0; i < L1I_TIMING_MSHR_SIZE; i++)
        {
          if (l1i_timing_mshr_table[i].tag ==
            (line_addr & L1I_TIMING_MSHR_TAG_MASK) &&
            l1i_timing_mshr_table[i].valid)
            return i;
        }
        return L1I_TIMING_MSHR_SIZE;
      }

      uint64_t l1i_find_timing_cache_entry(uint64_t line_addr)
      {
        uint64_t i = line_addr % MAX_NUM_SET;
        for (uint32_t j = 0; j < MAX_NUM_WAY; j++)
        {
          if (l1i_timing_cache_table[i][j].tag ==
            ((line_addr >> L1I_SET_BITS) & L1I_TIMING_CACHE_TAG_MASK)
            && l1i_timing_cache_table[i][j].valid)
            return j;
        }
        return MAX_NUM_WAY;
      }

      uint32_t l1i_get_invalid_timing_mshr_entry()
      {
        for (uint32_t i = 0; i < L1I_TIMING_MSHR_SIZE; i++)
        {
          if (!l1i_timing_mshr_table[i].valid)
            return i;
        }
        assert(false); // It must return a free entry
        return L1I_TIMING_MSHR_SIZE;
      }

      uint32_t l1i_get_invalid_timing_cache_entry(uint64_t line_addr)
      {
        uint32_t i = line_addr % MAX_NUM_SET;
        for (uint32_t j = 0; j < MAX_NUM_WAY; j++)
        {
          if (!l1i_timing_cache_table[i][j].valid)
            return j;
        }
        assert(false); // It must return a free entry
        return MAX_NUM_WAY;
      }

      void l1i_add_timing_entry(uint64_t line_addr,
        uint32_t source_set, uint32_t source_way)
      {
        // First find for coalescing
        if (l1i_find_timing_mshr_entry(line_addr) < L1I_TIMING_MSHR_SIZE)
          return;
        if (l1i_find_timing_cache_entry(line_addr) < MAX_NUM_WAY)
          return;

        uint32_t i = l1i_get_invalid_timing_mshr_entry();
        l1i_current_cycle = get_l1i_current_cycle();
        l1i_timing_mshr_table[i].valid = true;
        l1i_timing_mshr_table[i].tag = line_addr
                  & L1I_TIMING_MSHR_TAG_MASK;
        l1i_timing_mshr_table[i].source_set = source_set;
        l1i_timing_mshr_table[i].source_way = source_way;
        l1i_timing_mshr_table[i].timestamp = l1i_current_cycle
                  & L1I_TIME_MASK;
        l1i_timing_mshr_table[i].accessed = false;

      }

      void l1i_invalid_timing_mshr_entry(uint64_t line_addr)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        assert(index < L1I_TIMING_MSHR_SIZE);
        l1i_timing_mshr_table[index].valid = false;
      }

      void l1i_move_timing_entry(uint64_t line_addr)
      {
        uint32_t index_mshr = l1i_find_timing_mshr_entry(line_addr);
        if (index_mshr == L1I_TIMING_MSHR_SIZE)
        {
          uint32_t set = line_addr % MAX_NUM_SET;
          uint32_t index_cache = l1i_get_invalid_timing_cache_entry(line_addr);
          l1i_timing_cache_table[set][index_cache].valid = true;
          l1i_timing_cache_table[set][index_cache].tag =
                 (line_addr >> L1I_SET_BITS) & L1I_TIMING_CACHE_TAG_MASK;
          l1i_timing_cache_table[set][index_cache].source_way = L1I_ET_WAYS;
          l1i_timing_cache_table[set][index_cache].accessed = true;
          return;
        }
        uint64_t set = line_addr % MAX_NUM_SET;
        uint64_t index_cache = l1i_get_invalid_timing_cache_entry(line_addr);
        l1i_timing_cache_table[set][index_cache].valid = true;
        l1i_timing_cache_table[set][index_cache].tag =
        (line_addr >> L1I_SET_BITS) & L1I_TIMING_CACHE_TAG_MASK;
        l1i_timing_cache_table[set][index_cache].source_set =
                        l1i_timing_mshr_table[index_mshr].source_set;
        l1i_timing_cache_table[set][index_cache].source_way =
                       l1i_timing_mshr_table[index_mshr].source_way;
        l1i_timing_cache_table[set][index_cache].accessed =
                       l1i_timing_mshr_table[index_mshr].accessed;
        l1i_invalid_timing_mshr_entry(line_addr);
      }

      // returns if accessed
      bool l1i_invalid_timing_cache_entry(uint64_t line_addr,
          uint32_t &source_set, uint32_t &source_way)
      {
        uint32_t set = line_addr % MAX_NUM_SET;
        uint32_t way = l1i_find_timing_cache_entry(line_addr);
        assert(way < MAX_NUM_WAY);
        l1i_timing_cache_table[set][way].valid = false;
        source_set = l1i_timing_cache_table[set][way].source_set;
        source_way = l1i_timing_cache_table[set][way].source_way;
        return l1i_timing_cache_table[set][way].accessed;
      }

      void l1i_access_timing_entry(uint64_t line_addr, uint32_t pos_hist)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        if (index < L1I_TIMING_MSHR_SIZE)
        {
          if (!l1i_timing_mshr_table[index].accessed)
          {
            l1i_timing_mshr_table[index].accessed = true;
            l1i_timing_mshr_table[index].pos_hist = pos_hist;
          }
          return;
        }
        uint32_t set = line_addr % MAX_NUM_SET;
        uint32_t way = l1i_find_timing_cache_entry(line_addr);
        if (way < MAX_NUM_WAY)
        {
          l1i_timing_cache_table[set][way].accessed = true;
        }
      }

      bool l1i_different_timing_entry_src(uint64_t line_addr,
                     uint32_t source_set, uint32_t source_way)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        if (index < L1I_TIMING_MSHR_SIZE)
          if (l1i_timing_mshr_table[index].source_set != source_set ||
                      l1i_timing_mshr_table[index].source_way != source_way)
            return true;
        uint32_t set = line_addr % MAX_NUM_SET;
        uint32_t way = l1i_find_timing_cache_entry(line_addr);
        if (way < MAX_NUM_WAY)
          if (l1i_timing_cache_table[set][way].source_set != source_set ||
                    l1i_timing_cache_table[set][way].source_way != source_way)
            return true;
        return false;
      }

      bool l1i_is_accessed_timing_entry(uint64_t line_addr)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        if (index < L1I_TIMING_MSHR_SIZE)
        {
          return l1i_timing_mshr_table[index].accessed;
        }
        uint32_t set = line_addr % MAX_NUM_SET;
        uint32_t way = l1i_find_timing_cache_entry(line_addr);
        if (way < MAX_NUM_WAY)
        {
          return l1i_timing_cache_table[set][way].accessed;
        }
        return false;
      }

      bool l1i_completed_request(uint64_t line_addr)
      {
        return l1i_find_timing_cache_entry(line_addr) < MAX_NUM_WAY;
      }

      bool l1i_ongoing_request(uint64_t line_addr)
      {
        return l1i_find_timing_mshr_entry(line_addr)
            < L1I_TIMING_MSHR_SIZE;
      }

      bool l1i_ongoing_accessed_request(uint64_t line_addr)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        if (index == L1I_TIMING_MSHR_SIZE)
          return false;
        return l1i_timing_mshr_table[index].accessed;
      }

      uint64_t l1i_get_latency_timing_mshr(uint64_t line_addr,
                   uint32_t &pos_hist)
      {
        uint32_t index = l1i_find_timing_mshr_entry(line_addr);
        if (index == L1I_TIMING_MSHR_SIZE)
          return 0;
        if (!l1i_timing_mshr_table[index].accessed)
          return 0;
        pos_hist = l1i_timing_mshr_table[index].pos_hist;
        l1i_current_cycle = get_l1i_current_cycle();
        return l1i_get_latency(l1i_current_cycle,
              l1i_timing_mshr_table[index].timestamp);
      }

      // ENTANGLED TABLE

      uint64_t l1i_hash(uint64_t line_addr)
      {
        return line_addr ^ (line_addr >> 2) ^ (line_addr >> 5);
        // return line_addr ^ (line_addr >> 18);
      }

      void l1i_init_entangled_table()
      {
        for (uint32_t i = 0; i < L1I_ET_SETS; i++)
        {
          for (uint32_t j = 0; j < L1I_ET_WAYS; j++)
          {
            l1i_entangled_table[i][j].tag = 0;
            l1i_entangled_table[i][j].format = L1I_ENTANGLED_NUM_FORMATS;
            for (uint32_t k = 0;
               k < L1I_MAX_ENTANGLED_PER_LINE; k++)
            {
              l1i_entangled_table[i][j].entangled_addr[k] = 0;
              l1i_entangled_table[i][j].entangled_conf[k] = 0;
            }
            l1i_entangled_table[i][j].bb_size = 0;
          }
          l1i_entangled_fifo[i] = 0;
        }
      }

      uint32_t l1i_get_way_entangled_table(uint64_t line_addr)
      {
        uint64_t tag = (l1i_hash(line_addr) >>
            L1I_ENTANGLED_TABLE_INDEX_BITS) & L1I_TAG_MASK;
        uint32_t set = l1i_hash(line_addr) % L1I_ET_SETS;
        for (uint32_t i = 0; i < L1I_ET_WAYS; i++)
        {
          if (l1i_entangled_table[set][i].tag == tag)
          { // Found
            return i;
          }
        }
        return L1I_ET_WAYS;
      }

      void l1i_try_realocate_evicted_in_available_entangled_table(uint32_t set)
      {
        uint64_t way = l1i_entangled_fifo[set];
        bool dest_free_way = true;
        for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
        {
          if (l1i_entangled_table[set][way].entangled_conf[k] >=
                  L1I_CONFIDENCE_COUNTER_THRESHOLD)
          {
            dest_free_way = false;
            break;
          }
        }
        if (dest_free_way && l1i_entangled_table[set][way].bb_size == 0)
          return;
        uint32_t free_way = way;
        bool free_with_size = false;
        for (uint32_t i = (way + 1) % L1I_ET_WAYS;
              i != way; i = (i + 1) % L1I_ET_WAYS)
        {
          bool dest_free = true;
          for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            if (l1i_entangled_table[set][i].entangled_conf[k] >=
                        L1I_CONFIDENCE_COUNTER_THRESHOLD)
            {
              dest_free = false;
              break;
            }
          }
          if (dest_free)
          {
            if (free_way == way)
            {
              free_way = i;
              free_with_size =
                  (l1i_entangled_table[set][i].bb_size != 0);
            }
            else if (free_with_size &&
                  l1i_entangled_table[set][i].bb_size == 0)
            {
              free_way = i;
              free_with_size = false;
              break;
            }
          }
        }
        if (free_way != way && ((!free_with_size) ||
                   (free_with_size && !dest_free_way)))
        { // Only evict if it has more information
          l1i_entangled_table[set][free_way].tag =
              l1i_entangled_table[set][way].tag;
          l1i_entangled_table[set][free_way].format =
              l1i_entangled_table[set][way].format;
          for (uint32_t k = 0;
                k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            l1i_entangled_table[set][free_way].entangled_addr[k] =
                     l1i_entangled_table[set][way].entangled_addr[k];
            l1i_entangled_table[set][free_way].entangled_conf[k] =
                    l1i_entangled_table[set][way].entangled_conf[k];
          }
          l1i_entangled_table[set][free_way].bb_size =
                      l1i_entangled_table[set][way].bb_size;
        }
      }

      void l1i_add_entangled_table(uint64_t line_addr, uint64_t entangled_addr)
      {
        uint64_t tag = (l1i_hash(line_addr)
              >> L1I_ENTANGLED_TABLE_INDEX_BITS)
                  & L1I_TAG_MASK;
        uint32_t set = l1i_hash(line_addr) % L1I_ET_SETS;
        uint32_t way = l1i_get_way_entangled_table(line_addr);
        if (way == L1I_ET_WAYS)
        {
          l1i_try_realocate_evicted_in_available_entangled_table(set);
          way = l1i_entangled_fifo[set];
          l1i_entangled_table[set][way].tag = tag;
          l1i_entangled_table[set][way].format =
                  L1I_ENTANGLED_NUM_FORMATS;
          for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            l1i_entangled_table[set][way].entangled_addr[k] = 0;
            l1i_entangled_table[set][way].entangled_conf[k] = 0;
          }
          l1i_entangled_table[set][way].bb_size = 0;
          l1i_entangled_fifo[set] = (l1i_entangled_fifo[set] + 1) %
                          L1I_ET_WAYS;
        }
        for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
        {
          if (l1i_entangled_table[set][way].entangled_conf[k] >=
            L1I_CONFIDENCE_COUNTER_THRESHOLD &&
            l1i_extend_format_entangled(line_addr,
              l1i_entangled_table[set][way].entangled_addr[k],
              l1i_entangled_table[set][way].format) == entangled_addr)
          {
            l1i_entangled_table[set][way].entangled_conf[k] =
                   L1I_CONFIDENCE_COUNTER_MAX_VALUE;
            return;
          }
        }

        // Adding a new entangled
        uint32_t format_new = l1i_get_format_entangled(line_addr,
              entangled_addr);

        // Check for evictions
        while (true)
        {
          uint32_t min_format = format_new;
          uint32_t num_valid = 1;
          uint32_t min_value = L1I_CONFIDENCE_COUNTER_MAX_VALUE + 1;
          uint32_t min_pos = 0;
          for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            if (l1i_entangled_table[set][way].entangled_conf[k] >=
                      L1I_CONFIDENCE_COUNTER_THRESHOLD)
            {
              num_valid++;
              uint32_t format_k = l1i_get_format_entangled(line_addr,
                  l1i_extend_format_entangled(
                    line_addr,l1i_entangled_table[set][way].entangled_addr[k],
                    l1i_entangled_table[set][way].format)
                  );
              if (format_k < min_format)
              {
                min_format = format_k;
              }
              if (l1i_entangled_table[set][way].entangled_conf[k] < min_value)
              {
                min_value = l1i_entangled_table[set][way].entangled_conf[k];
                min_pos = k;
              }
            }
          }
          if (num_valid > min_format)
          { // Eviction is necessary. We chose the lower confidence one
            l1i_entangled_table[set][way].entangled_conf[min_pos] = 0;
          }
          else
          {
            // Reformat
            for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
            {
              if (l1i_entangled_table[set][way].entangled_conf[k]
                                >= L1I_CONFIDENCE_COUNTER_THRESHOLD)
              {
                l1i_entangled_table[set][way].entangled_addr[k] =
                    l1i_compress_format_entangled(l1i_extend_format_entangled
                      (
                       line_addr,
                       l1i_entangled_table[set][way].entangled_addr[k],
                       l1i_entangled_table[set][way].format), min_format
                      );
              }
            }
            l1i_entangled_table[set][way].format = min_format;
            break;
          }
        }
        for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
        {
          if (l1i_entangled_table[set][way].entangled_conf[k] <
                L1I_CONFIDENCE_COUNTER_THRESHOLD)
          {
            l1i_entangled_table[set][way].entangled_addr[k] =
                l1i_compress_format_entangled(entangled_addr,
                    l1i_entangled_table[set][way].format);
            l1i_entangled_table[set][way].entangled_conf[k] =
                L1I_CONFIDENCE_COUNTER_MAX_VALUE;
            return;
          }
        }
      }

      void l1i_add_bbsize_table(uint64_t line_addr, uint32_t bb_size)
      {
        assert(bb_size <= L1I_MERGE_BBSIZE_MAX_VALUE);
        uint64_t tag = (l1i_hash(line_addr)
                >> L1I_ENTANGLED_TABLE_INDEX_BITS) & L1I_TAG_MASK;
        uint32_t set = l1i_hash(line_addr) % L1I_ET_SETS;
        uint32_t way = l1i_get_way_entangled_table(line_addr);
        if (way == L1I_ET_WAYS)
        {
          l1i_try_realocate_evicted_in_available_entangled_table(set);
          way = l1i_entangled_fifo[set];
          l1i_entangled_table[set][way].tag = tag;
          l1i_entangled_table[set][way].format = L1I_ENTANGLED_NUM_FORMATS;
          for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            l1i_entangled_table[set][way].entangled_addr[k] = 0;
            l1i_entangled_table[set][way].entangled_conf[k] = 0;
          }
          l1i_entangled_table[set][way].bb_size = 0;
          l1i_entangled_fifo[set] = (l1i_entangled_fifo[set] + 1)
                                          % L1I_ET_WAYS;
        }
        l1i_entangled_table[set][way].bb_size =
              std::max(l1i_entangled_table[set][way].bb_size, bb_size);
      }

      uint64_t l1i_get_entangled_addr_entangled_table(uint64_t line_addr,
                              uint32_t index_k, uint32_t &set, uint32_t &way)
      {
        set = l1i_hash(line_addr) % L1I_ET_SETS;
        way = l1i_get_way_entangled_table(line_addr);
        if (way < L1I_ET_WAYS)
        {
          if (l1i_entangled_table[set][way].entangled_conf[index_k]
                      >= L1I_CONFIDENCE_COUNTER_THRESHOLD)
          {
            return l1i_extend_format_entangled(line_addr,
              l1i_entangled_table[set][way].entangled_addr[index_k],
              l1i_entangled_table[set][way].format);
          }
        }
        return 0;
      }

      uint32_t l1i_get_bbsize_entangled_table(uint64_t line_addr)
      {
        uint32_t set = l1i_hash(line_addr) % L1I_ET_SETS;
        uint32_t way = l1i_get_way_entangled_table(line_addr);
        if (way < L1I_ET_WAYS)
        {
          return l1i_entangled_table[set][way].bb_size;
        }
        return 0;
      }

      void l1i_update_confidence_entangled_table(uint32_t set,
                    uint32_t way, uint64_t entangled_addr, bool accessed)
      {
        if (way < L1I_ET_WAYS)
        {
          for (uint32_t k = 0; k < L1I_MAX_ENTANGLED_PER_LINE; k++)
          {
            if (l1i_entangled_table[set][way].entangled_conf[k]
              >= L1I_CONFIDENCE_COUNTER_THRESHOLD &&
              l1i_compress_format_entangled(
                l1i_entangled_table[set][way].entangled_addr[k],
                l1i_entangled_table[set][way].format
              )
              == l1i_compress_format_entangled(
                entangled_addr,
                l1i_entangled_table[set][way].format
              )
            )
            {
              if (accessed && l1i_entangled_table[set][way].entangled_conf[k] <
                    L1I_CONFIDENCE_COUNTER_MAX_VALUE)
              {
                l1i_entangled_table[set][way].entangled_conf[k]++;
              }
              if (!accessed &&
                  l1i_entangled_table[set][way].entangled_conf[k] > 0)
              {
                l1i_entangled_table[set][way].entangled_conf[k]--;
              }
            }
          }
        }
      }

      void l1i_move_history_head_to_entangled()
      {
        // move bb_size when exiting the history
        if (l1i_hist_table[l1i_hist_table_head].bb_size)
        {
          uint32_t max_bb_size =
            l1i_get_bbsize_entangled_table(
              l1i_hist_table[l1i_hist_table_head].tag
            );
          l1i_add_bbsize_table(
            l1i_hist_table[l1i_hist_table_head].tag,
            std::max(max_bb_size,
              l1i_hist_table[l1i_hist_table_head].bb_size)
          );
        }

        // move entangling pair when exiting the history
        if (l1i_hist_table[l1i_hist_table_head].ent_src)
        {
          l1i_add_entangled_table(l1i_hist_table[l1i_hist_table_head].ent_src,
                 l1i_hist_table[l1i_hist_table_head].tag);
        }
      }

      // It can have duplicated entries if the line was evicted in between
      uint32_t l1i_add_hist_table(uint64_t line_addr,
        uint64_t instr_id,
        uint32_t format,
        uint32_t num_dst
      )
      {
        // Insert empty addresses in hist - to have timediff overflows
        l1i_current_cycle = get_l1i_current_cycle();
        while (l1i_current_cycle - l1i_hist_table_head_time
            >= L1I_TIME_DIFF_OVERFLOW)
        {

          l1i_move_history_head_to_entangled();

          l1i_hist_table[l1i_hist_table_head].tag = 0;
          l1i_hist_table[l1i_hist_table_head].time_diff = L1I_TIME_DIFF_MASK;
          l1i_hist_table[l1i_hist_table_head].bb_size = 0;
          l1i_hist_table[l1i_hist_table_head].instr_id = 0;
          l1i_hist_table[l1i_hist_table_head].ent_src = 0;
          l1i_hist_table[l1i_hist_table_head].format = 0;
          l1i_hist_table[l1i_hist_table_head].num_dst = 0;
          l1i_hist_table_head =
            (l1i_hist_table_head + 1) % L1I_HIST_TABLE_ENTRIES;
          l1i_hist_table_head_time += L1I_TIME_DIFF_MASK;
        }

        l1i_move_history_head_to_entangled();

        l1i_current_cycle = get_l1i_current_cycle();

        // Allocate a new entry (evict old one if necessary)
        l1i_hist_table[l1i_hist_table_head].tag =
            line_addr & L1I_HIST_TAG_MASK;
        l1i_hist_table[l1i_hist_table_head].time_diff =
                (l1i_current_cycle - l1i_hist_table_head_time)
                & L1I_TIME_DIFF_MASK;
        l1i_hist_table[l1i_hist_table_head].bb_size = 0;
        l1i_hist_table[l1i_hist_table_head].instr_id = instr_id;
        l1i_hist_table[l1i_hist_table_head].ent_src = 0;
        l1i_hist_table[l1i_hist_table_head].format = format;
        l1i_hist_table[l1i_hist_table_head].num_dst = num_dst;
        uint32_t pos = l1i_hist_table_head;
        l1i_hist_table_head = (l1i_hist_table_head + 1)
                  % L1I_HIST_TABLE_ENTRIES;
        l1i_hist_table_head_time = l1i_current_cycle;
        return pos;
      }

      struct pfInMSHRQ
      {
        uint64_t lineAddr;
        uint32_t sourceSet;
        uint32_t sourceWay;
      };

      std::deque<pfInMSHRQ> pfmq;

      void calculatePrefetch(const PrefetchInfo &pfi,
                             std::vector<AddrPriority> &addresses) override;

      void notifyFill(const PacketPtr &pkt) override;
      void notifySquash(const PacketPtr &pkt) override;
      void notifyInvalidate(const Addr &addr) override;


      void prefetch_issued_to_pq(uint64_t lineAddr,
               uint64_t sourceSet, uint64_t sourceWay) override;
      void add_pfmq(uint64_t lineAddr,
            uint32_t sourceSet, uint32_t sourceWay) override;
      void remove_pfmq(uint64_t lineAddr) override;
      void clear_pfmq() override;
      uint64_t getSourceSet(uint64_t lineAddr) override;
      uint64_t getSourceWay(uint64_t lineAddr) override;
    };

  } // namespace prefetch
} // namespace gem5

#endif // __MEM_CACHE_PREFETCH_TAGGED_HH__
