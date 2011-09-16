/*
 * Copyright (c) 2008, Julien Quintard
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of EPITA nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JULIEN QUINTARD ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL JULIEN QUINTARD BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef BPT_H
#define BPT_H

/*
 * ---------- includes --------------------------------------------------------
 */

#include <string.h>
#include <stdio.h>

/*
 * ---------- meta-information ------------------------------------------------
 */

/*
 * general defines
 */

#define BPT_VERSION		"2.60"

/*
 * init values and limitations
 */

#define BPT_INIT_HEIGHT		0x1
#define BPT_INIT_NIKEYS		0x2
#define BPT_INIT_NLKEYS		0x2

/*
 * ---------- types -----------------------------------------------------------
 */

/*
 * default types
 */

#define BPT_NDI_T		signed int
#define BPT_UNI_T		signed int
#define BPT_OPTS_T		unsigned char
#define BPT_CB_T		unsigned char
#define BPT_TYPE_T		unsigned char
#define BPT_FLAGS_T		unsigned char
#define BPT_NODES_T		unsigned int
#define BPT_HEIGHT_T		unsigned short
#define BPT_BALANCING_T		unsigned char

/*
 * general types
 */

typedef BPT_CB_T		t_bpt_cb;
typedef BPT_OPTS_T		t_bpt_opts;
typedef BPT_TYPE_T		t_bpt_type;
typedef BPT_FLAGS_T		t_bpt_flags;

/*
 * ---------- definitions -----------------------------------------------------
 */

/*
 * options
 */

#define BPT_OPT_ZERO		0x0

#define BPT_OPT_TREE		0x1
#define BPT_OPT_NODE		0x2

#define BPT_OPT_HEAD		0x1
#define BPT_OPT_TAIL		0x2

#define BPT_OPT_KEY		0x1
#define BPT_OPT_PARENT		0x2

#define BPT_OPT_PERFECT		0x1
#define BPT_OPT_PARTIAL		0x2
#define BPT_OPT_COLLIDE		0x4

#define BPT_OPT_INIT		0x1
#define BPT_OPT_ADD		0x2
#define BPT_OPT_MODIFY		0x3
#define BPT_OPT_REMOVE		0x4
#define BPT_OPT_CLEAN		0x5

#define BPT_BALANCING_PLUS	50
#define BPT_BALANCING_STAR	75

/*
 * callbacks
 */

#define BPT_CB_UNKNOWN		0x0
#define BPT_CB_INSERT		0x1
#define BPT_CB_SPLIT		0x2
#define BPT_CB_MODIFY		0x3
#define BPT_CB_DELETE		0x4
#define BPT_CB_MIGRATE		0x5
#define BPT_CB_BALANCE		0x6

/*
 * flags
 */

#define BPT_FLAG_ZERO		0x0
#define BPT_FLAG_CALLBACK	0x1
#define BPT_FLAG_COLLIDE	0x2

/*
 * types
 */

#define BPT_TYPE_INTERNAL	0x01
#define BPT_TYPE_LEAF		0x02

/*
 * ---------- debug macro -----------------------------------------------------
 */

#ifdef BPT_DEBUG
# define	bpt_debug(_stream_, _fmt_...)                                 \
  {                                                                           \
    fprintf((_stream_), "[%s:%u] ", __FILE__, __LINE__);                      \
    fprintf((_stream_), ##_fmt_);                                             \
  }
#else
# define	bpt_debug(_stream_, _fmt_...)
#endif

/*
 * ---------- size-macros -----------------------------------------------------
 */

/*
 * macros that indicate the number of array's entries the programmer has to
 * fill with blocks addresses before performing common tree's operations
 * such as: init, add, modify etc.
 *
 * for the remove operations, no blocks addresses are needed but an array
 * has to be provided since bpt will fill its entries with the addresses
 * of the blocks that are no longer used.
 *
 * for the init operation, a block is needed for creating the root node.
 *
 * the clean operation does not use any allocation but need an array for
 * returning all the blocks.
 */

#define		BPT_INIT_ALLOC()                                              \
  (1)

#define		BPT_ADD_ALLOC(_bpt_)                                          \
  ((_bpt_)->height + 1)

#define		BPT_MODIFY_ALLOC(_bpt_)                                       \
  (BPT_ADD_ALLOC(_bpt_) + BPT_REMOVE_ALLOC(_bpt_))

#define		BPT_REMOVE_ALLOC(_bpt_)                                       \
  (0)

#define		BPT_CLEAN_ALLOC(_bpt_)                                        \
  (0)

/*
 * macros that specify the size of the array bpt needs as parameter.
 */

#define		BPT_INIT_SIZE()                                               \
  (1)

#define		BPT_ADD_SIZE(_bpt_)                                           \
  ((_bpt_)->height + 1)

#define		BPT_MODIFY_SIZE(_bpt_)                                        \
  (BPT_ADD_SIZE(_bpt_) + BPT_REMOVE_SIZE(_bpt_))

#define		BPT_REMOVE_SIZE(_bpt_)                                        \
  (_bpt_)->height

#define		BPT_CLEAN_SIZE(_bpt_)                                         \
  ((_bpt_)->nodes)

/*
 * ---------- type-macros -----------------------------------------------------
 */

/*
 * user friendly macros
 */

#define		t_bpt(T)                                                      \
  t_bpt_##T

#define		t_bpt_addr(T)                                                 \
  t_bpt_addr_##T

#define		t_bpt_key(T)                                                  \
  t_bpt_key_##T

#define		t_bpt_value(T)                                                \
  t_bpt_value_##T

#define		t_bpt_ndi(T)                                                  \
  t_bpt_ndi_##T

#define		t_bpt_uni(T)                                                  \
  t_bpt_uni_##T

#define		t_bpt_nodes(T)                                                \
  t_bpt_nodes_##T

#define		t_bpt_height(T)                                               \
  t_bpt_height_##T

#define		t_bpt_node(T)                                                 \
  t_bpt_node_##T

#define		t_bpt_imm(T)                                                  \
  t_bpt_imm_##T

#define		t_bpt_cbctx(T)                                                \
  t_bpt_cbctx_##T

#define		t_bpt_unused(T)                                               \
  t_bpt_unused_##T

#define		t_bpt_head(T)                                                 \
  t_bpt_head_##T

#define		t_bpt_inentry(T)                                              \
  t_bpt_inentry_##T

#define		t_bpt_lfentry(T)                                              \
  t_bpt_lfentry_##T

#define		t_bpt_entry(T)                                                \
  t_bpt_entry_##T

#define		t_bpt_load_fn(T)                                              \
  t_bpt_load_fn_##T

#define		t_bpt_unload_fn(T)                                            \
  t_bpt_unload_fn_##T

#define		t_bpt_addrcmp_fn(T)                                           \
  t_bpt_addrcmp_fn_##T

#define		t_bpt_keycmp_fn(T)                                            \
  t_bpt_keycmp_fn_##T

#define		t_bpt_valcmp_fn(T)                                            \
  t_bpt_valcmp_fn_##T

#define		t_bpt_cb_fn(T)                                                \
  t_bpt_cb_fn_##T

/*
 * ---------- traps -----------------------------------------------------------
 */

#define		bpt_foreach(T, _bpt_, _entry_)                                \
  bpt_foreach_##T(_bpt_, _entry_)

#define		bpt_list(T, _bpt_, _node_, _entry_, _opts_)                   \
  bpt_list_##T(_bpt_, _node_, _entry_, _opts_)

#define		bpt_check_unused(T, _bpt_, _unused_, _opts_)                  \
  bpt_check_unused_##T(_bpt_, _unused_, _opts_)

#define		bpt_first_entry(T, _bpt_, _node_, _first_)                    \
  bpt_first_entry_##T(_bpt_, _node_, _first_)

#define		bpt_prev_entry(T, _bpt_, _current_, _previous_, _opts_)       \
  bpt_prev_entry_##T(_bpt_, _current_, _previous_, _opts_)

#define		bpt_next_entry(T, _bpt_, _current_, _next_, _opts_)           \
  bpt_next_entry_##T(_bpt_, _current_, _next_, _opts_)

#define		bpt_last_entry(T, _bpt_, _node_, _last_)                      \
  bpt_last_entry_##T(_bpt_, _node_, _last_)

#define		bpt_reinit_entries(T, _bpt_, _node_)                          \
  bpt_reinit_entries_##T(_bpt_, _node_)

#define		bpt_make_node(T, _bpt_, _node_, _type_)                       \
  bpt_make_node_##T(_bpt_, _node_, _type_)

#define		bpt_key(T, _bpt_, _node_, _key_)                              \
  bpt_key_##T(_bpt_, _node_, _key_)

#define		bpt_ndi(T, _bpt_, _node_, _value_, _ndi_)                     \
  bpt_ndi_##T(_bpt_, _node_, _value_, _ndi_)

#define		bpt_update_node(T, _bpt_, _node_, _addr_, _key_,_opts_)       \
  bpt_update_node_##T(_bpt_, _node_, _addr_, _key_, _opts_)

#define		bpt_update_parent(T, _bpt_, _node_)                           \
  bpt_update_parent_##T(_bpt_, _node_)

#define		bpt_update(T, _bpt_, _node_, _opts_)                          \
  bpt_update_##T(_bpt_, _node_, _opts_)

#define		bpt_linear_search(T, _bpt_, _node_, _key_, _ndi_, _opts_)     \
  bpt_linear_search_##T(_bpt_, _node_, _key_, _ndi_, _opts_)

#define		bpt_dichotomic2_search(T, _bpt_, _node_, _key_, _ndi_, _opts_)\
  bpt_dichotomic2_search_##T(_bpt_, _node_, _key_, _ndi_, _opts_)

#define		bpt_dichotomic_search(T, _bpt_, _node_, _key_, _ndi_, _opts_) \
  bpt_dichotomic_search_##T(_bpt_, _node_, _key_, _ndi_, _opts_)

#define		bpt_search_entry(T, _bpt_, _node_, _key_, _ndi_, _opts_)      \
  bpt_search_entry_##T(_bpt_, _node_, _key_, _ndi_, _opts_)

#define		bpt_search_leaf(T, _bpt_, _node_, _leaf_, _key_)              \
  bpt_search_leaf_##T(_bpt_, _node_, _leaf_, _key_)

#define		bpt_search(T, _bpt_, _key_, _entry_)                          \
  bpt_search_##T(_bpt_, _key_, _entry_)

#define		bpt_collide_next(T, _bpt_, _key_, _entry_)                    \
  bpt_collide_next_##T(_bpt_, _key_, _entry_)

#define		bpt_collide_search(T, _bpt_, _key_, _value_, _entry_)         \
  bpt_collide_search_##T(_bpt_, _key_, _value_, _entry_)

#define		bpt_check_collide(T, _bpt_, _node1_, _key_, _value_)          \
  bpt_check_collide_##T(_bpt_, _node1_, _key_, _value_)

#define		bpt_node_size(T, _bpt_, _node_)                               \
  bpt_node_size_##T(_bpt_, _node_)

#define		bpt_simplify(T, _bpt_, _node_, _unused_)                      \
  bpt_simplify_##T(_bpt_, _node_, _unused_)

#define		bpt_balancein_1(T, _bpt_, _node1_, _node2_,                   \
                                _cbctx_, _prev_, _unused_)                    \
  bpt_balancein_1_##T(_bpt_, _node1_, _node2_, _cbctx_, _prev_, _unused_)

#define		bpt_balancein_2(T, _bpt_, _node1_, _node2_,                   \
                                _cbctx_, _prev_, _unused_)                    \
  bpt_balancein_2_##T(_bpt_, _node1_, _node2_, _cbctx_, _prev_, _unused_)

#define		bpt_balancein_3(T, _bpt_, _node1_, _node2_,                   \
                                _cbctx_, _prev_, _unused_)                    \
  bpt_balancein_3_##T(_bpt_, _node1_, _node2_, _cbctx_, _prev_, _unused_)

#define		bpt_balancein_4(T, _bpt_, _node1_, _node2_,                   \
                                _cbctx_, _prev_, _unused_)                    \
  bpt_balancein_4_##T(_bpt_, _node1_, _node2_, _cbctx_, _prev_, _unused_)

#define		bpt_delete(T, _bpt_, _entry_, _cbctx_, _unused_)              \
  bpt_delete_##T(_bpt_, _entry_, _cbctx_, _unused_)

#define		bpt_remove(T, _bpt_, _key_, _unused_)                         \
  bpt_remove_##T(_bpt_, _key_, _unused_)

#define		bpt_collide_remove(T, _bpt_, _entry_, _unused_)               \
  bpt_collide_remove_##T(_bpt_, _entry_, _unused_)

#define		bpt_modify(T, _bpt_, _key_, _lfentry_, _unused_)              \
  bpt_modify_##T(_bpt_, _key_, _lfentry_, _unused_)

#define		bpt_collide_modify(T, _bpt_, _entry_, _lfentry_, _unused_)    \
  bpt_collide_modify_##T(_bpt_, _entry_, _lfentry_, _unused_)

#define		bpt_insert_head(T, _bpt_, _node1_, _node2_)                   \
  bpt_insert_head_##T(_bpt_, _node1_, _node2_)

#define		bpt_insert_tail(T, _bpt_, _node1_, _node2_)                   \
  bpt_insert_tail_##T(_bpt_, _node1_, _node2_)

#define		bpt_shift_sort(T, _bpt_, _node_)                              \
  bpt_shift_sort_##T(_bpt_, _node_)

#define		bpt_insert_sort(T, _bpt_, _node_, _key_, _ndi_, _opts_)       \
  bpt_insert_sort_##T(_bpt_, _node_, _key_, _ndi_, _opts_)

#define		bpt_new_root(T, _bpt_, _node1_, _node2_, _unused_)            \
  bpt_new_root_##T(_bpt_, _node1_, _node2_, _unused_)

#define		bpt_balanceout(T, _bpt_, _node1_, _node2_, _entry_, _cbctx_)  \
  bpt_balanceout_##T(_bpt_, _node1_, _node2_, _entry_, _cbctx_)

#define		bpt_split_node(T, _bpt_, _node1_, _entry_,                    \
                               _current_, _unused_)                           \
  bpt_split_node_##T(_bpt_, _node1_, _entry_, _current_, _unused_)

#define		bpt_insert(T, _bpt_, _node_, _entry_, _cbctx_, _unused_)      \
  bpt_insert_##T(_bpt_, _node_, _entry_, _cbctx_, _unused_)

#define		bpt_add(T, _bpt_, _lfentry_, _unused_)                        \
  bpt_add_##T(_bpt_, _lfentry_, _unused_)

#define		bpt_init(T, _bpt_, _nodesz_, _uaddr_, _ukey_, _uval_,         \
                         _flags_, _balancing_, _load_, _unload_, _addrcmp_,   \
                         _keycmp_, _valcmp_, _callback_, _data_, _unused_)    \
  bpt_init_##T(_bpt_, _nodesz_, _uaddr_, _ukey_, _uval_, _flags_, _balancing_,\
               _load_, _unload_, _addrcmp_, _keycmp_, _valcmp_, _callback_,   \
               _data_, _unused_)

#define		bpt_clean_node(T, _bpt_, _node_, _unused_)                    \
  bpt_clean_node_##T(_bpt_, _node_, _unused_)

#define		bpt_clean(T, _bpt_, _unused_)                                 \
  bpt_clean_##T(_bpt_, _unused_)

/*
 * ---------- template-dependent macros ---------------------------------------
 */

#define		BPT_GET_HEAD(T, _node_, _elem_)                               \
  ((t_bpt_head(T) *)((_node_)->buf))->_elem_

#define		BPT_SET_HEAD(T, _node_, _elem_, _value_)                      \
  ((t_bpt_head(T) *)((_node_)->buf))->_elem_ = (_value_)

#define		BPT_INENTRY(T, _node_, _ndi_)                                 \
  (t_bpt_inentry(T) *)((_node_)->buf + sizeof(t_bpt_head(T)) +                \
                       (_ndi_) * sizeof(t_bpt_inentry(T)))

#define		BPT_LFENTRY(T, _node_, _ndi_)                                 \
  (t_bpt_lfentry(T) *)((_node_)->buf + sizeof(t_bpt_head(T)) +                \
                       (_ndi_) * sizeof(t_bpt_lfentry(T)))

#define		BPT_INIT_ENTRY(T, _node_, _ndi_)                              \
  memset(BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_inentry(T)) :                                 \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_lfentry(T)), 0x0,                             \
         BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         sizeof(t_bpt_inentry(T)) : sizeof(t_bpt_lfentry(T)))

#define		BPT_IMPORT_ENTRY(T, _node_, _ndi_, _entry_)                   \
  memcpy(BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_inentry(T)) :                                 \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_lfentry(T)),                                  \
         (_entry_),                                                           \
         BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         sizeof(t_bpt_inentry(T)) : sizeof(t_bpt_lfentry(T)))

#define		BPT_IMPORT_INENTRY(T, _node_, _ndi_, _entry_)                 \
  memcpy((_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_inentry(T)),                                  \
         (_entry_),                                                           \
         sizeof(t_bpt_inentry(T)))

#define		BPT_IMPORT_LFENTRY(T, _node_, _ndi_, _entry_)                 \
  memcpy((_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_lfentry(T)),                                  \
         (_entry_),                                                           \
         sizeof(t_bpt_lfentry(T)))

#define		BPT_EXPORT_ENTRY(T, _node_, _ndi_, _entry_)                   \
  memcpy((_entry_),                                                           \
         BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_inentry(T)) :                                 \
         (_node_)->buf + sizeof(t_bpt_head(T)) +                              \
         (_ndi_) * sizeof(t_bpt_lfentry(T)),                                  \
         BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?               \
         sizeof(t_bpt_inentry(T)) : sizeof(t_bpt_lfentry(T)))

#define		BPT_COPY_ENTRY(T, _node1_, _node2_, _ndi1_, _ndi2_)           \
  memcpy(BPT_GET_HEAD(T, (_node1_), type) == BPT_TYPE_INTERNAL ?              \
         (_node2_)->buf + sizeof(t_bpt_head(T)) +                             \
         (_ndi2_) * sizeof(t_bpt_inentry(T)) :                                \
         (_node2_)->buf + sizeof(t_bpt_head(T)) +                             \
         (_ndi2_) * sizeof(t_bpt_lfentry(T)),                                 \
         BPT_GET_HEAD(T, (_node1_), type) == BPT_TYPE_INTERNAL ?              \
         (_node1_)->buf + sizeof(t_bpt_head(T)) +                             \
         (_ndi1_) * sizeof(t_bpt_inentry(T)) :                                \
         (_node1_)->buf + sizeof(t_bpt_head(T)) +                             \
         (_ndi1_) * sizeof(t_bpt_lfentry(T)),                                 \
         BPT_GET_HEAD(T, (_node1_), type) == BPT_TYPE_INTERNAL ?              \
         sizeof(t_bpt_inentry(T)) : sizeof(t_bpt_lfentry(T)))

#define		BPT_SWAP_ENTRIES(T, _node_, _ndi1_, _ndi2_)                   \
  {                                                                           \
    if (BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL)                 \
      {                                                                       \
        t_bpt_inentry(T)	_swap_;                                       \
                                                                              \
        memcpy(&_swap_, (_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi1_) *    \
               sizeof(t_bpt_inentry(T)), sizeof(t_bpt_inentry(T)));           \
        memcpy((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi1_) *             \
               sizeof(t_bpt_inentry(T)), (_node_)->buf +                      \
               sizeof(t_bpt_head(T)) + (_ndi2_) *                             \
               sizeof(t_bpt_inentry(T)), sizeof(t_bpt_inentry(T)));           \
        memcpy((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi2_) *             \
               sizeof(t_bpt_inentry(T)), &_swap_,                             \
               sizeof(t_bpt_inentry(T)));                                     \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        t_bpt_lfentry(T)	_swap_;                                       \
                                                                              \
        memcpy(&_swap_, (_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi1_) *    \
               sizeof(t_bpt_lfentry(T)), sizeof(t_bpt_lfentry(T)));           \
        memcpy((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi1_) *             \
               sizeof(t_bpt_lfentry(T)), (_node_)->buf +                      \
               sizeof(t_bpt_head(T)) + (_ndi2_) *                             \
               sizeof(t_bpt_lfentry(T)), sizeof(t_bpt_lfentry(T)));           \
        memcpy((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi2_) *             \
               sizeof(t_bpt_lfentry(T)), &_swap_,                             \
               sizeof(t_bpt_lfentry(T)));                                     \
      }                                                                       \
  }

#define		BPT_GET_ENTRY(T, _node_, _ndi_, _elem_)                       \
  (BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?                     \
   ((t_bpt_inentry(T) *)((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi_) *    \
    sizeof(t_bpt_inentry(T))))->_elem_ :                                      \
   ((t_bpt_lfentry(T) *)((_node_)->buf + sizeof(t_bpt_head(T)) + (_ndi_) *    \
    sizeof(t_bpt_lfentry(T))))->_elem_)

#define		BPT_GET_INENTRY(T, _node_, _ndi_, _elem_)                     \
  (BPT_INENTRY(T, (_node_), (_ndi_)))->_elem_

#define		BPT_GET_LFENTRY(T, _node_, _ndi_, _elem_)                     \
  (BPT_LFENTRY(T, (_node_), (_ndi_)))->_elem_

#define		BPT_SET_ENTRY(T, _node_, _ndi_, _elem_, _value_)              \
  {                                                                           \
    if (BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL)                 \
      {                                                                       \
        t_bpt_inentry(T)	*_entry_ = BPT_INENTRY(T, _node_, _ndi_);     \
                                                                              \
        _entry_->_elem_ = (_value_);                                          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        t_bpt_lfentry(T)	*_entry_ = BPT_LFENTRY(T, _node_, _ndi_);     \
                                                                              \
        _entry_->_elem_ = (_value_);                                          \
      }                                                                       \
  }

#define		BPT_SET_INENTRY(T, _node_, _ndi_, _elem_, _value_)            \
  {                                                                           \
    t_bpt_inentry(T)	*_entry_ = BPT_INENTRY(T, _node_, _ndi_);             \
                                                                              \
    _entry_->_elem_ = (_value_);                                              \
  }

#define		BPT_SET_LFENTRY(T, _node_, _ndi_, _elem_, _value_)            \
  {                                                                           \
    t_bpt_lfentry(T)	*_entry_ = BPT_LFENTRY(T, _node_, _ndi_);             \
                                                                              \
    _entry_->_elem_ = (_value_);                                              \
  }

#define		BPT_GET_THIS_ENTRY(T, _type_, _ptr_, _elem_)                  \
  ((_type_) == BPT_TYPE_INTERNAL ?                                            \
   ((t_bpt_inentry(T) *)(_ptr_))->_elem_ :                                    \
   ((t_bpt_lfentry(T) *)(_ptr_))->_elem_)

#define		BPT_BALANCING(T, _bpt_, _node_)                               \
  (t_bpt_ndi(T)) (BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?      \
   ((_bpt_)->ibalancing) :                                                    \
   ((_bpt_)->lbalancing))

#define		BPT_NKEYS(T, _bpt_, _node_)                                   \
  (BPT_GET_HEAD(T, (_node_), type) == BPT_TYPE_INTERNAL ?                     \
   (_bpt_)->nikeys : (_bpt_)->nlkeys)

/*
 * ---------- template-independent macros -------------------------------------
 */

#define		BPT_NODES(_bpt_)                                              \
  (_bpt_)->nodes

#define		BPT_NIKEYS(_bpt_)                                             \
  (_bpt_)->nikeys

#define		BPT_NLKEYS(_bpt_)                                             \
  (_bpt_)->nlkeys

#define		BPT_HEIGHT(_bpt_)                                             \
  (_bpt_)->height

#define		BPT_COPY_NODE(_bpt_, _node1_, _node2_)                        \
  memcpy((_node2_), (_node1_), (_bpt_)->nodesz)

#define		BPT_LOAD(_bpt_, _node_, _addr_)                               \
  (_bpt_)->load((_bpt_), (_node_), (_addr_))

#define		BPT_UNLOAD(_bpt_, _node_)                                     \
  (_bpt_)->unload((_bpt_), (_node_));

#define		BPT_ADDRCMP(_bpt_, _addr1_, _addr2_)                          \
  (_bpt_)->addrcmp((_bpt_), (_addr1_), (_addr2_))

#define		BPT_KEYCMP(_bpt_, _key1_, _key2_)                             \
  (_bpt_)->keycmp((_bpt_), (_key1_), (_key2_))

#define		BPT_VALCMP(_bpt_, _val1_, _val2_)                             \
  (_bpt_)->valcmp((_bpt_), (_val1_), (_val2_))

#define		BPT_CALLBACK(_bpt_, _cbctx_)                                  \
  {                                                                           \
    if ((_bpt_)->flags & BPT_FLAG_CALLBACK)                                   \
      (_bpt_)->callback((_bpt_), (_cbctx_));                                  \
  }

#define		BPT_GET_ROOT(_bpt_)                                           \
  (_bpt_)->root

#define		BPT_SET_ROOT(_bpt_, _root_)                                   \
  (_bpt_)->root = (_root_)

#define		BPT_INIT_CBCTX(_bpt_, _cbctx_)                                \
  {                                                                           \
    if ((_bpt_)->flags & BPT_FLAG_CALLBACK)                                   \
      {                                                                       \
        (_cbctx_)->cb = BPT_CB_UNKNOWN;                                       \
                                                                              \
        (_cbctx_)->previous.node = (_bpt_)->uaddr;                            \
        (_cbctx_)->previous.ndi = 0;                                          \
        (_cbctx_)->current.node = (_bpt_)->uaddr;                             \
        (_cbctx_)->current.ndi = 0;                                           \
                                                                              \
        (_cbctx_)->node = bpt->uaddr;                                         \
                                                                              \
        (_cbctx_)->node1 = bpt->uaddr;                                        \
        (_cbctx_)->node2 = bpt->uaddr;                                        \
      }                                                                       \
  }

#define		BPT_SET_CBCTX(_bpt_, _type_, _cbctx_, _elem_, _value_)        \
  {                                                                           \
    if (((_bpt_)->flags & BPT_FLAG_CALLBACK) &&                               \
        ((_type_) == BPT_TYPE_LEAF))                                          \
      (_cbctx_)->_elem_ = (_value_);                                          \
  }

#define		BPT_RESERVE(_bpt_, _unused_, _var_)                           \
  {                                                                           \
    (_var_) = (_unused_)->array[(_unused_)->index];                           \
    (_unused_)->array[(_unused_)->index] = bpt->uaddr;                        \
    (_unused_)->index--;                                                      \
    (_bpt_)->nodes++;                                                         \
  }

#define		BPT_RELEASE(_bpt_, _unused_, _var_)                           \
  {                                                                           \
    (_bpt_)->nodes--;                                                         \
    (_unused_)->index++;                                                      \
    (_unused_)->array[(_unused_)->index] = (_var_);                           \
  }

/*
 * ---------- types generator -------------------------------------------------
 */

#define		bpt_make_types(T, _t_bpt_ndi_, _t_bpt_uni_, _t_bpt_nodes_,    \
                               _t_bpt_height_, _t_bpt_addr_, _t_bpt_key_,     \
                               _t_bpt_value_, _t_bpt_inentry_,                \
                               _t_bpt_lfentry_)                               \
                                                                              \
typedef struct s_bpt_##T	t_bpt_##T;                                    \
                                                                              \
typedef _t_bpt_ndi_		t_bpt_ndi_##T;                                \
typedef _t_bpt_uni_		t_bpt_uni_##T;                                \
typedef _t_bpt_nodes_		t_bpt_nodes_##T;                              \
typedef _t_bpt_height_		t_bpt_height_##T;                             \
typedef _t_bpt_addr_		t_bpt_addr_##T;                               \
typedef _t_bpt_key_		t_bpt_key_##T;                                \
typedef _t_bpt_value_		t_bpt_value_##T;                              \
typedef _t_bpt_inentry_		t_bpt_inentry_##T;                            \
typedef _t_bpt_lfentry_		t_bpt_lfentry_##T;                            \
                                                                              \
typedef t_bpt_addr(T)		t_bpt_node_##T;                               \
                                                                              \
typedef struct			s_bpt_imm_##T                                 \
{                                                                             \
  t_bpt_node(T)			addr;                                         \
  void				*buf;                                         \
}				t_bpt_imm_##T;                                \
                                                                              \
typedef struct			s_bpt_entry_##T                               \
{                                                                             \
  t_bpt_node(T)			node;                                         \
  t_bpt_ndi(T)			ndi;                                          \
}				t_bpt_entry_##T;                              \
                                                                              \
typedef struct			s_bpt_cbctx_##T                               \
{                                                                             \
  t_bpt_cb			cb;                                           \
                                                                              \
  t_bpt_entry(T)		previous;                                     \
  t_bpt_entry(T)		current;                                      \
                                                                              \
  t_bpt_node(T)			node;                                         \
                                                                              \
  t_bpt_node(T)			node1;                                        \
  t_bpt_node(T)			node2;                                        \
}				t_bpt_cbctx_##T;                              \
                                                                              \
typedef void	(*t_bpt_load_fn_##T)(t_bpt(T)			*bpt,         \
				     t_bpt_imm(T)		*node,        \
				     t_bpt_node(T)		addr);        \
                                                                              \
typedef void	(*t_bpt_unload_fn_##T)(t_bpt(T)			*bpt,         \
				       t_bpt_imm(T)		*node);       \
                                                                              \
typedef int	(*t_bpt_addrcmp_fn_##T)(t_bpt(T)		*bpt,         \
				       t_bpt_addr(T)		addr1,        \
				       t_bpt_addr(T)		addr2);       \
                                                                              \
typedef int	(*t_bpt_keycmp_fn_##T)(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key1,         \
				       t_bpt_key(T)		key2);        \
                                                                              \
typedef int	(*t_bpt_valcmp_fn_##T)(t_bpt(T)			*bpt,         \
				       t_bpt_value(T)		val1,         \
				       t_bpt_value(T)		val2);        \
                                                                              \
typedef void	(*t_bpt_cb_fn_##T)(t_bpt(T)			*bpt,         \
				   t_bpt_cbctx(T)		*cbctx);      \
                                                                              \
struct				s_bpt_##T                                     \
{                                                                             \
  t_bpt_ndi(T)			nodesz;                                       \
  t_bpt_nodes(T)		nodes;                                        \
  t_bpt_ndi(T)			nikeys;                                       \
  t_bpt_ndi(T)			nlkeys;                                       \
  t_bpt_height(T)		height;                                       \
  t_bpt_node(T)			root;                                         \
  t_bpt_flags			flags;                                        \
  t_bpt_ndi(T)			ibalancing;                                   \
  t_bpt_ndi(T)			lbalancing;                                   \
                                                                              \
  t_bpt_addr(T)			uaddr;                                        \
  t_bpt_key(T)			ukey;                                         \
  t_bpt_value(T)		uval;                                         \
                                                                              \
  t_bpt_load_fn(T)		load;                                         \
  t_bpt_unload_fn(T)		unload;                                       \
  t_bpt_addrcmp_fn(T)		addrcmp;                                      \
  t_bpt_keycmp_fn(T)		keycmp;                                       \
  t_bpt_valcmp_fn(T)		valcmp;                                       \
  t_bpt_cb_fn(T)		callback;                                     \
                                                                              \
  void				*data;                                        \
};                                                                            \
                                                                              \
typedef struct			s_bpt_unused_##T                              \
{                                                                             \
  t_bpt_node(T)			*array;                                       \
  t_bpt_uni(T)			index;                                        \
}				t_bpt_unused_##T;                             \
                                                                              \
typedef struct			s_bpt_head_##T                                \
{                                                                             \
  t_bpt_type			type;                                         \
  t_bpt_node(T)			parent;                                       \
  t_bpt_node(T)			prv;                                          \
  t_bpt_node(T)			nxt;                                          \
}				__attribute__((packed)) t_bpt_head_##T;       

/*
 * ---------- prototypes generator --------------------------------------------
 */

#define		bpt_make_protos(T)                                            \
                                                                              \
int			bpt_foreach_##T(t_bpt(T)                    *bpt,     \
                                        t_bpt_entry(T)              *entry);  \
                                                                              \
int			bpt_list_##T(t_bpt(T)			*bpt,         \
				     t_bpt_imm(T)		*node,        \
				     t_bpt_entry(T)		*entry,       \
				     t_bpt_opts			opts);        \
                                                                              \
int			bpt_check_unused_##T(t_bpt(T)		*bpt,         \
					     t_bpt_unused(T)	*unused,      \
					     t_bpt_opts		opts);        \
                                                                              \
int			bpt_first_entry_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_ndi(T)	*first);      \
                                                                              \
int			bpt_prev_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_entry(T)	current,      \
				           t_bpt_entry(T)	*previous,    \
				           t_bpt_opts		opts);        \
                                                                              \
int			bpt_next_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_entry(T)	current,      \
				           t_bpt_entry(T)	*next,        \
				           t_bpt_opts		opts);        \
                                                                              \
int			bpt_last_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node,        \
				           t_bpt_ndi(T)		*last);       \
                                                                              \
void			bpt_reinit_entries_##T(t_bpt(T)		*bpt,         \
					       t_bpt_imm(T)	*node);       \
                                                                              \
void			bpt_make_node_##T(t_bpt(T)		*bpt,         \
				          t_bpt_imm(T)		*node,        \
				          t_bpt_type		type);        \
                                                                              \
void			bpt_key_##T(t_bpt(T)			*bpt,         \
				    t_bpt_imm(T)		*node,        \
				    t_bpt_key(T)		*key);        \
                                                                              \
                                                                              \
int			bpt_ndi_##T(t_bpt(T)			*bpt,         \
				    t_bpt_imm(T)		*node,        \
				    t_bpt_addr(T)		value,        \
				    t_bpt_ndi(T)		*ndi);        \
                                                                              \
int			bpt_update_node_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_node(T)	addr,         \
					    t_bpt_key(T)	key,          \
					    t_bpt_opts		opts);        \
                                                                              \
void			bpt_update_parent_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node);       \
                                                                              \
int			bpt_update_##T(t_bpt(T)			*bpt,         \
				       t_bpt_imm(T)		*node,        \
				       t_bpt_opts		opts);        \
                                                                              \
int			bpt_linear_search_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node,        \
					      t_bpt_key(T)	key,          \
					      t_bpt_ndi(T)	*ndi,         \
					      t_bpt_opts	opts);        \
                                                                              \
int			bpt_dichotomic_search_##T(t_bpt(T)	*bpt,         \
					          t_bpt_imm(T)	*node,        \
					          t_bpt_key(T)	key,          \
					          t_bpt_ndi(T)	*ndi,         \
					          t_bpt_opts	opts);        \
                                                                              \
int			bpt_dichotomic2_search_##T(t_bpt(T)	*bpt,         \
					          t_bpt_imm(T)	*node,        \
					          t_bpt_key(T)	key,          \
					          t_bpt_ndi(T)	*ndi,         \
					          t_bpt_opts	opts);        \
                                                                              \
int			bpt_search_entry_##T(t_bpt(T)		*bpt,         \
					     t_bpt_imm(T)	*node,        \
					     t_bpt_key(T)	key,          \
					     t_bpt_ndi(T)	*ndi,         \
					     t_bpt_opts		opts);        \
                                                                              \
int			bpt_search_leaf_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_node(T)	*leaf,        \
					    t_bpt_key(T)	key);         \
                                                                              \
int			bpt_search_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_entry(T)		*entry);      \
                                                                              \
int			bpt_collide_next_##T(t_bpt(T)		*bpt,         \
					     t_bpt_key(T)	key,          \
					     t_bpt_entry(T)	*entry);      \
                                                                              \
int			bpt_collide_search_##T(t_bpt(T)		*bpt,         \
					       t_bpt_key(T)	key,          \
					       t_bpt_value(T)	value,        \
					       t_bpt_entry(T)	*entry);      \
                                                                              \
int			bpt_check_collide_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node1,       \
					      t_bpt_key(T)	key,          \
					      t_bpt_value(T)	value);       \
                                                                              \
t_bpt_ndi(T)		bpt_node_size_##T(t_bpt(T)		*bpt,         \
					  t_bpt_imm(T)		*node);       \
                                                                              \
void			bpt_simplify_##T(t_bpt(T)		*bpt,         \
					 t_bpt_imm(T)		*node,        \
					 t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_balancein_1_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_balancein_2_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_balancein_3_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_balancein_4_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_delete_##T(t_bpt(T)			*bpt,         \
				       t_bpt_entry(T)		entry,        \
				       t_bpt_cbctx(T)		*cbctx,       \
				       t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_remove_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_collide_remove_##T(t_bpt(T)		*bpt,         \
					       t_bpt_entry(T)	entry,        \
					       t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_modify_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_lfentry(T)		*lfentry,     \
				       t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_collide_modify_##T(t_bpt(T)		*bpt,         \
					       t_bpt_entry(T)	entry,        \
					       t_bpt_lfentry(T)	*lfentry,     \
					       t_bpt_unused(T)	*unused);     \
                                                                              \
void			bpt_insert_head_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2);      \
                                                                              \
void			bpt_insert_tail_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2);      \
                                                                              \
void			bpt_shift_sort_##T(t_bpt(T)		*bpt,         \
					   t_bpt_imm(T)		*node);       \
                                                                              \
void			bpt_insert_sort_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_key(T)	key,          \
					    t_bpt_ndi(T)	*ndi,         \
					    t_bpt_opts		opts);        \
                                                                              \
void			bpt_new_root_##T(t_bpt(T)		*bpt,         \
					 t_bpt_imm(T)		*node1,       \
					 t_bpt_imm(T)		*node2,       \
					 t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_balanceout_##T(t_bpt(T)		*bpt,         \
					   t_bpt_imm(T)		*node1,       \
					   t_bpt_imm(T)		*node2,       \
					   void			*entry,       \
					   t_bpt_cbctx(T)	*cbctx);      \
                                                                              \
int			bpt_split_node_##T(t_bpt(T)		*bpt,         \
					   t_bpt_imm(T)		*node1,       \
					   void			*entry,       \
					   t_bpt_cbctx(T)	*cbctx,       \
					   t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_insert_##T(t_bpt(T)			*bpt,         \
				       t_bpt_imm(T)		*node,        \
				       void			*entry,       \
				       t_bpt_cbctx(T)		*cbctx,       \
				       t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_add_##T(t_bpt(T)			*bpt,         \
				    t_bpt_lfentry(T)		*lfentry,     \
				    t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_init_##T(t_bpt(T)			*bpt,         \
				     t_bpt_ndi(T)		nodesz,       \
				     t_bpt_addr(T)		uaddr,        \
				     t_bpt_key(T)		ukey,         \
				     t_bpt_value(T)		uval,         \
				     t_bpt_flags		flags,        \
				     BPT_BALANCING_T		balancing,    \
				     t_bpt_load_fn(T)		load,         \
				     t_bpt_unload_fn(T)		unload,       \
				     t_bpt_addrcmp_fn(T)	addrcmp,      \
				     t_bpt_keycmp_fn(T)		keycmp,       \
				     t_bpt_valcmp_fn(T)		valcmp,       \
				     t_bpt_cb_fn(T)		callback,     \
				     void			*data,        \
				     t_bpt_unused(T)		*unused);     \
                                                                              \
int			bpt_clean_node_##T(t_bpt(T)		*bpt,         \
					   t_bpt_imm(T)		*node,        \
					   t_bpt_unused(T)	*unused);     \
                                                                              \
int			bpt_clean_##T(t_bpt(T)			*bpt,         \
				      t_bpt_unused(T)		*unused);     

/*
 * ---------- functions generator ---------------------------------------------
 */

#define		bpt_make_functions(T, _key_, _value_)                         \
                                                                              \
/*                                                                            \
 * this function iterates on the entries stored in the tree.                  \
 */                                                                           \
                                                                              \
int			bpt_foreach_##T(t_bpt(T)		*bpt,         \
				        t_bpt_entry(T)		*entry)       \
{                                                                             \
  if (BPT_ADDRCMP(bpt, entry->node, bpt->uaddr) == 0)                         \
    {                                                                         \
      t_bpt_imm(T)	node;                                                 \
                                                                              \
      BPT_LOAD(bpt, &node, BPT_GET_ROOT(bpt));                                \
                                                                              \
      if (bpt_list(T, bpt, &node, entry, BPT_OPT_HEAD) != 0)                  \
	{                                                                     \
	  BPT_UNLOAD(bpt, &node);                                             \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  if (bpt_next_entry(T, bpt, *entry, entry, BPT_OPT_TREE) != 0)               \
    return (-1);                                                              \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function finds the head/tail entry in the leaf linked list.           \
 */                                                                           \
                                                                              \
int			bpt_list_##T(t_bpt(T)			*bpt,         \
				     t_bpt_imm(T)		*node,        \
				     t_bpt_entry(T)		*entry,       \
				     t_bpt_opts			opts)         \
{                                                                             \
  t_bpt_imm(T)		child;                                                \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  if (BPT_GET_HEAD(T, node, type) == BPT_TYPE_LEAF)                           \
    {                                                                         \
      entry->node = node->addr;                                               \
      if (opts & BPT_OPT_HEAD)                                                \
	{                                                                     \
	  if (bpt_first_entry(T, bpt, node, &entry->ndi) != 0)                \
	    return (-1);                                                      \
	}                                                                     \
      else if (opts & BPT_OPT_TAIL)                                           \
	{                                                                     \
	  if (bpt_last_entry(T, bpt, node, &entry->ndi) != 0)                 \
	    return (-1);                                                      \
	}                                                                     \
      else                                                                    \
	return (-1);                                                          \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_HEAD)                                                    \
    {                                                                         \
      if (bpt_first_entry(T, bpt, node, &i) != 0)                             \
	return (-1);                                                          \
    }                                                                         \
  else if (opts & BPT_OPT_TAIL)                                               \
    {                                                                         \
      if (bpt_last_entry(T, bpt, node, &i) != 0)                              \
	return (-1);                                                          \
    }                                                                         \
  else                                                                        \
    return (-1);                                                              \
                                                                              \
  BPT_LOAD(bpt, &child, BPT_GET_INENTRY(T, node, i, _value_));                \
                                                                              \
  if (bpt_list(T, bpt, &child, entry, opts) != 0)                             \
    {                                                                         \
      BPT_UNLOAD(bpt, &child);                                                \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &child);                                                    \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function checks whether the _unused_ array was correctly built.       \
 */                                                                           \
                                                                              \
int			bpt_check_unused_##T(t_bpt(T)		*bpt,         \
					     t_bpt_unused(T)	*unused,      \
					     t_bpt_opts		opts)         \
{                                                                             \
  if (opts == BPT_OPT_INIT)                                                   \
    {                                                                         \
      if (unused->index >= (BPT_INIT_ALLOC() - 1))                            \
	return (0);                                                           \
    }                                                                         \
  else if (opts == BPT_OPT_ADD)                                               \
    {                                                                         \
      if (unused->index >= (BPT_ADD_ALLOC(bpt) - 1))                          \
	return (0);                                                           \
    }                                                                         \
  else if (opts == BPT_OPT_MODIFY)                                            \
    {                                                                         \
      if (unused->index >= (BPT_MODIFY_ALLOC(bpt) - 1))                       \
	return (0);                                                           \
    }                                                                         \
  else if (opts == BPT_OPT_REMOVE)                                            \
    {                                                                         \
      if (unused->index >= (BPT_REMOVE_ALLOC(bpt) - 1))                       \
	return (0);                                                           \
    }                                                                         \
  else if (opts == BPT_OPT_CLEAN)                                             \
    {                                                                         \
      if (unused->index >= (BPT_CLEAN_ALLOC(bpt) - 1))                        \
	return (0);                                                           \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this functions returns the first entry of the node.                        \
 */                                                                           \
                                                                              \
int			bpt_first_entry_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_ndi(T)	*first)       \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0; i < nkeys; i++)                                                 \
    if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, i, _key_), bpt->ukey) != 0)    \
      {                                                                       \
	*first = i;                                                           \
	return (0);                                                           \
      }                                                                       \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function returns the previous entry.                                  \
 */                                                                           \
                                                                              \
int			bpt_prev_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_entry(T)	current,      \
				           t_bpt_entry(T)	*previous,    \
				           t_bpt_opts		opts)         \
{                                                                             \
  if (opts & BPT_OPT_NODE)                                                    \
    {                                                                         \
      t_bpt_ndi(T)	nkeys;                                                \
      t_bpt_imm(T)	node;                                                 \
      t_bpt_ndi(T)	i;                                                    \
                                                                              \
      if ((current.ndi - 1) < 0)                                              \
	return (-1);                                                          \
                                                                              \
      BPT_LOAD(bpt, &node, current.node);                                     \
      nkeys = BPT_NKEYS(T, bpt, &node);                                       \
                                                                              \
      if (current.ndi >= nkeys)                                               \
	{                                                                     \
	  BPT_UNLOAD(bpt, &node);                                             \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      for (i = current.ndi - 1; i >= 0; i--)                                  \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, &node, i, _key_),                \
                       bpt->ukey) != 0)                                       \
	  {                                                                   \
	    previous->node = node.addr;                                       \
	    previous->ndi = i;                                                \
                                                                              \
	    BPT_UNLOAD(bpt, &node);                                           \
                                                                              \
	    return (0);                                                       \
	  }                                                                   \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_TREE)                                                    \
    {                                                                         \
      t_bpt_imm(T)	node;                                                 \
      t_bpt_imm(T)	prev;                                                 \
                                                                              \
      if (bpt_prev_entry(T, bpt, current, previous, BPT_OPT_NODE) == 0)       \
	return (0);                                                           \
                                                                              \
      do                                                                      \
	{                                                                     \
	  BPT_LOAD(bpt, &node, current.node);                                 \
                                                                              \
	  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node, prv), bpt->uaddr) == 0) \
	    {                                                                 \
	      BPT_UNLOAD(bpt, &node);                                         \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  BPT_LOAD(bpt, &prev, BPT_GET_HEAD(T, &node, prv));                  \
	  BPT_UNLOAD(bpt, &node);                                             \
                                                                              \
	  previous->node = prev.addr;                                         \
	  if (bpt_last_entry(T, bpt, &prev, &previous->ndi) == 0)             \
	    {                                                                 \
	      BPT_UNLOAD(bpt, &prev);                                         \
	      return (0);                                                     \
	    }                                                                 \
                                                                              \
	  current.node = prev.addr;                                           \
                                                                              \
	  BPT_UNLOAD(bpt, &prev);                                             \
	} while (1);                                                          \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function returns the next entry.                                      \
 */                                                                           \
                                                                              \
int			bpt_next_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_entry(T)	current,      \
				           t_bpt_entry(T)	*next,        \
				           t_bpt_opts		opts)         \
{                                                                             \
  if (opts & BPT_OPT_NODE)                                                    \
    {                                                                         \
      t_bpt_ndi(T)	nkeys;                                                \
      t_bpt_imm(T)	node;                                                 \
      t_bpt_ndi(T)	i;                                                    \
                                                                              \
      BPT_LOAD(bpt, &node, current.node);                                     \
      nkeys = BPT_NKEYS(T, bpt, &node);                                       \
                                                                              \
      if ((current.ndi >= nkeys) ||                                           \
	  ((current.ndi + 1) >= nkeys))                                       \
	{                                                                     \
	  BPT_UNLOAD(bpt, &node);                                             \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      for (i = current.ndi + 1; i < nkeys; i++)                               \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, &node, i, _key_),                \
                       bpt->ukey) != 0)                                       \
	  {                                                                   \
	    next->node = node.addr;                                           \
	    next->ndi = i;                                                    \
                                                                              \
	    BPT_UNLOAD(bpt, &node);                                           \
                                                                              \
	    return (0);                                                       \
	  }                                                                   \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_TREE)                                                    \
    {                                                                         \
      t_bpt_imm(T)	node;                                                 \
      t_bpt_imm(T)	nxt;                                                  \
                                                                              \
      if (bpt_next_entry(T, bpt, current, next, BPT_OPT_NODE) == 0)           \
	return (0);                                                           \
                                                                              \
      do                                                                      \
	{                                                                     \
	  BPT_LOAD(bpt, &node, current.node);                                 \
                                                                              \
	  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node, nxt), bpt->uaddr) == 0) \
	    {                                                                 \
	      BPT_UNLOAD(bpt, &node);                                         \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  BPT_LOAD(bpt, &nxt, BPT_GET_HEAD(T, &node, nxt));                   \
	  BPT_UNLOAD(bpt, &node);                                             \
                                                                              \
	  next->node = nxt.addr;                                              \
	  if (bpt_first_entry(T, bpt, &nxt, &next->ndi) == 0)                 \
	    {                                                                 \
	      BPT_UNLOAD(bpt, &nxt);                                          \
	      return (0);                                                     \
	    }                                                                 \
                                                                              \
	  current.node = nxt.addr;                                            \
                                                                              \
	  BPT_UNLOAD(bpt, &nxt);                                              \
	} while (1);                                                          \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function returns the last entry of the node.                          \
 */                                                                           \
                                                                              \
int			bpt_last_entry_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node,        \
				           t_bpt_ndi(T)		*last)        \
{                                                                             \
  if ((*last = bpt_node_size(T, bpt, node) - 1) < 0)                          \
    return (-1);                                                              \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function simply reinitializes entries.                                \
 */                                                                           \
                                                                              \
void			bpt_reinit_entries_##T(t_bpt(T)		*bpt,         \
					       t_bpt_imm(T)	*node)        \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0; i < nkeys; i++)                                                 \
    {                                                                         \
      BPT_INIT_ENTRY(T, node, i);                                             \
      BPT_SET_ENTRY(T, node, i, _key_, bpt->ukey);                            \
                                                                              \
      if (BPT_GET_HEAD(T, node, type) == BPT_TYPE_INTERNAL)                   \
        {                                                                     \
          BPT_SET_INENTRY(T, node, i, _value_, bpt->uaddr);                   \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          BPT_SET_LFENTRY(T, node, i, _value_, bpt->uval);                    \
        }                                                                     \
    }                                                                         \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function creates and initializes a node.                              \
 */                                                                           \
                                                                              \
void			bpt_make_node_##T(t_bpt(T)		*bpt,         \
				          t_bpt_imm(T)		*node,        \
				          t_bpt_type		type)         \
{                                                                             \
  t_bpt_ndi(T)		nkeys;                                                \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  nkeys = type == BPT_TYPE_INTERNAL ? bpt->nikeys : bpt->nlkeys;              \
                                                                              \
  BPT_SET_HEAD(T, node, type, type);                                          \
  BPT_SET_HEAD(T, node, parent, bpt->uaddr);                                  \
  BPT_SET_HEAD(T, node, prv, bpt->uaddr);                                     \
  BPT_SET_HEAD(T, node, nxt, bpt->uaddr);                                     \
                                                                              \
  for (i = 0; i < nkeys; i++)                                                 \
    {                                                                         \
      BPT_INIT_ENTRY(T, node, i);                                             \
      BPT_SET_ENTRY(T, node, i, _key_, bpt->ukey);                            \
                                                                              \
      if (BPT_GET_HEAD(T, node, type) == BPT_TYPE_INTERNAL)                   \
        {                                                                     \
          BPT_SET_INENTRY(T, node, i, _value_, bpt->uaddr);                   \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          BPT_SET_LFENTRY(T, node, i, _value_, bpt->uval);                    \
        }                                                                     \
    }                                                                         \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for the highest key present in a node.                 \
 */                                                                           \
                                                                              \
void			bpt_key_##T(t_bpt(T)			*bpt,         \
				    t_bpt_imm(T)		*node,        \
				    t_bpt_key(T)		*key)         \
{                                                                             \
  t_bpt_ndi(T)		ndi;                                                  \
                                                                              \
  bpt_last_entry(T, bpt, node, &ndi);                                         \
                                                                              \
  *key = BPT_GET_ENTRY(T, node, ndi, _key_);                                  \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function tries to find the most appropriate entry that matches        \
 * with the parameter value. finally the function returns the node index.     \
 *                                                                            \
 * the value field is used because two entries could collide i.e have the     \
 * same key. however, two internal node's entries cannot have the same        \
 * value as value represents the address of a child block. obviously,         \
 * a tree cannot have two entries leading to the same block as this is        \
 * a graph.                                                                   \
 *                                                                            \
 * the value field is therefore used to retrieve an entry among collisions.   \
 */                                                                           \
                                                                              \
int			bpt_ndi_##T(t_bpt(T)			*bpt,         \
				    t_bpt_imm(T)		*node,        \
				    t_bpt_addr(T)		value,        \
				    t_bpt_ndi(T)		*ndi)         \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0; (i < nkeys) &&                                                  \
	 (BPT_KEYCMP(bpt, BPT_GET_INENTRY(T, node, i, _key_),                 \
		     bpt->ukey) != 0);                                        \
       i++)                                                                   \
    if (BPT_ADDRCMP(bpt, BPT_GET_INENTRY(T, node, i, _value_), value) == 0)   \
      {                                                                       \
	*ndi = i;                                                             \
	return (0);                                                           \
      }                                                                       \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function updates the key field.                                       \
 */                                                                           \
                                                                              \
int			bpt_update_node_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_node(T)	addr,         \
					    t_bpt_key(T)	key,          \
					    t_bpt_opts		opts)         \
{                                                                             \
  t_bpt_key(T)		old = bpt->ukey;                                      \
  t_bpt_key(T)		new = bpt->ukey;                                      \
  t_bpt_imm(T)		parent;                                               \
  t_bpt_ndi(T)		ndi;                                                  \
                                                                              \
  if (bpt_ndi(T, bpt, node, addr, &ndi) != 0)                                 \
    {                                                                         \
      bpt_debug(stderr, "[bptd] panic: this case should never happen\n");     \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_KEY)                                                     \
    {                                                                         \
      bpt_key(T, bpt, node, &old);                                            \
      BPT_SET_ENTRY(T, node, ndi, _key_, key);                                \
    }                                                                         \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node, parent), bpt->uaddr) == 0)       \
    return (0);                                                               \
                                                                              \
  bpt_key(T, bpt, node, &new);                                                \
                                                                              \
  if (!(opts & BPT_OPT_KEY))                                                  \
    old = new;                                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, new, old) == 0)                                         \
    return (0);                                                               \
                                                                              \
  BPT_LOAD(bpt, &parent, BPT_GET_HEAD(T, node, parent));                      \
                                                                              \
  if (bpt_update_node(T, bpt, &parent, node->addr, new, opts) != 0)           \
    {                                                                         \
      BPT_UNLOAD(bpt, &parent);                                               \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &parent);                                                   \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function updates the parent field of each children.                   \
 */                                                                           \
                                                                              \
void			bpt_update_parent_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node)        \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0; (i < nkeys) &&                                                  \
	 (BPT_KEYCMP(bpt, BPT_GET_INENTRY(T, node, i, _key_),                 \
		     bpt->ukey) != 0);                                        \
       i++)                                                                   \
    {                                                                         \
      t_bpt_imm(T)	child;                                                \
                                                                              \
      BPT_LOAD(bpt, &child, BPT_GET_INENTRY(T, node, i, _value_));            \
      BPT_SET_HEAD(T, &child, parent, node->addr);                            \
      BPT_UNLOAD(bpt, &child);                                                \
    }                                                                         \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function recursively updates the parent node.                         \
 */                                                                           \
                                                                              \
int			bpt_update_##T(t_bpt(T)			*bpt,         \
				       t_bpt_imm(T)		*node,        \
				       t_bpt_opts		opts)         \
{                                                                             \
  t_bpt_imm(T)		parent;                                               \
  t_bpt_key(T)		key;                                                  \
                                                                              \
  if (opts & BPT_OPT_PARENT)                                                  \
    {                                                                         \
      bpt_update_parent(T, bpt, node);                                        \
      opts &= ~BPT_OPT_PARENT;                                                \
    }                                                                         \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node, parent), bpt->uaddr) == 0)	      \
    return (0);                                                               \
                                                                              \
  if (opts == BPT_OPT_ZERO)                                                   \
    return (0);                                                               \
                                                                              \
  bpt_key(T, bpt, node, &key);                                                \
                                                                              \
  BPT_LOAD(bpt, &parent, BPT_GET_HEAD(T, node, parent));                      \
                                                                              \
  if (bpt_update_node(T, bpt, &parent, node->addr, key, opts) != 0)           \
    {                                                                         \
      BPT_UNLOAD(bpt, &parent);                                               \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &parent);                                                   \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function linearly searches for the given key.                         \
 */                                                                           \
                                                                              \
int			bpt_linear_search_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node,        \
					      t_bpt_key(T)	key,          \
					      t_bpt_ndi(T)	*ndi,         \
					      t_bpt_opts	opts)         \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0; (i < nkeys) &&                                                  \
	 (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, i, _key_), bpt->ukey) != 0); \
       i++)                                                                   \
    {                                                                         \
      if (opts & BPT_OPT_PARTIAL)                                             \
	if ((BPT_KEYCMP(bpt, key, BPT_GET_ENTRY(T, node, i, _key_)) <= 0) ||  \
	    ((i + 1) >= nkeys) ||                                             \
	    (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, i + 1, _key_),            \
                        bpt->ukey) == 0))                                     \
	  {                                                                   \
	    *ndi = i;                                                         \
	    return (0);                                                       \
	  }                                                                   \
                                                                              \
      if (opts & BPT_OPT_PERFECT)                                             \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, i, _key_), key) == 0)      \
	  {                                                                   \
            *ndi = i;                                                         \
	    return (0);                                                       \
	  }                                                                   \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for a key using a dichotomic technique and manage      \
 * collisions safely                                                          \
 */                                                                           \
                                                                              \
int			bpt_dichotomic2_search_##T(t_bpt(T)	*bpt,         \
						   t_bpt_imm(T)	*node,        \
						   t_bpt_key(T)	key,          \
						   t_bpt_ndi(T)	*ndi,         \
						   t_bpt_opts	opts)         \
{                                                                             \
  t_bpt_ndi(T)		j = BPT_NKEYS(T, bpt, node) - 1;                      \
  t_bpt_ndi(T)		i = 0;                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, 0, _key_), bpt->ukey) == 0)      \
    {                                                                         \
      *ndi = 0;                                                               \
      return (opts & BPT_OPT_PARTIAL ? 0 : -1);                               \
    }                                                                         \
                                                                              \
  while (i <= j)                                                              \
    {                                                                         \
      t_bpt_ndi(T)	k = (i + j) / 2;                                      \
                                                                              \
      if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_), bpt->ukey) == 0)  \
	{                                                                     \
	  j = k - 1;                                                          \
	  continue;                                                           \
	}                                                                     \
                                                                              \
      if (opts & BPT_OPT_PERFECT)                                             \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_), key) == 0)      \
          {                                                                   \
            while (1)                                                         \
              {                                                               \
                if (k == 0)                                                   \
                  {                                                           \
                    *ndi = 0;                                                 \
                    return (0);                                               \
                  }                                                           \
                if (BPT_KEYCMP(bpt,                                           \
                               BPT_GET_ENTRY(T, node, k - 1, _key_),          \
                               key) == 0)                                     \
                  {                                                           \
                    k = k - 1;                                                \
                  }                                                           \
                else                                                          \
                  {                                                           \
                    *ndi = k;                                                 \
                    return (0);                                               \
                  }                                                           \
              }                                                               \
           }                                                                  \
      if (opts & BPT_OPT_PARTIAL)                                             \
        if ((BPT_KEYCMP(bpt, key, BPT_GET_ENTRY(T, node, k, _key_)) <= 0) &&  \
            ((k == 0) ||                                                      \
             (BPT_KEYCMP(bpt, key,                                            \
                         BPT_GET_ENTRY(T, node, k - 1, _key_)) > 0)))         \
          {                                                                   \
            if (k == 0)                                                       \
              {                                                               \
                *ndi = 0;                                                     \
                return (0);                                                   \
              }                                                               \
            while (1)                                                         \
              {                                                               \
                if (k == 1)                                                   \
                  {                                                           \
                    *ndi = 1;                                                 \
                    return (0);                                               \
                  }                                                           \
                if ((BPT_KEYCMP(bpt,                                          \
                                key,                                          \
                                BPT_GET_ENTRY(T, node, k - 1, _key_)) <= 0) &&\
                    (BPT_KEYCMP(bpt,                                          \
                                key,                                          \
                                BPT_GET_ENTRY(T, node, k - 2, _key_)) > 0))   \
                  {                                                           \
                    k = k - 1;                                                \
                  }                                                           \
                else                                                          \
                  {                                                           \
                    *ndi = k;                                                 \
                    return (0);                                               \
                  }                                                           \
             }                                                                \
          }                                                                   \
                                                                              \
      if (BPT_KEYCMP(bpt, key, BPT_GET_ENTRY(T, node, k, _key_)) < 0)         \
	j = k - 1;                                                            \
      else                                                                    \
	i = k + 1;                                                            \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_PARTIAL)                                                 \
    {                                                                         \
      if (bpt_last_entry(T, bpt, node, ndi) != 0)                             \
	return (-1);                                                          \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for a key using a dichotomic technique.                \
 */                                                                           \
                                                                              \
int			bpt_dichotomic_search_##T(t_bpt(T)	*bpt,         \
					          t_bpt_imm(T)	*node,        \
					          t_bpt_key(T)	key,          \
					          t_bpt_ndi(T)	*ndi,         \
					          t_bpt_opts	opts)         \
{                                                                             \
  t_bpt_ndi(T)		j = BPT_NKEYS(T, bpt, node) - 1;                      \
  t_bpt_ndi(T)		i = 0;                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, 0, _key_), bpt->ukey) == 0)      \
    {                                                                         \
      *ndi = 0;                                                               \
      return (opts & BPT_OPT_PARTIAL ? 0 : -1);                               \
    }                                                                         \
                                                                              \
  while (i <= j)                                                              \
    {                                                                         \
      t_bpt_ndi(T)	k = (i + j) / 2;                                      \
                                                                              \
      if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_), bpt->ukey) == 0)  \
	{                                                                     \
	  j = k - 1;                                                          \
	  continue;                                                           \
	}                                                                     \
                                                                              \
      if (opts & BPT_OPT_PERFECT)                                             \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_), key) == 0)      \
	  {                                                                   \
	    *ndi = k;                                                         \
	    return (0);                                                       \
	  }                                                                   \
                                                                              \
      if (opts & BPT_OPT_PARTIAL)                                             \
	if ((BPT_KEYCMP(bpt, key, BPT_GET_ENTRY(T, node, k, _key_)) <= 0) &&  \
	    ((k == 0) ||                                                      \
	     (BPT_KEYCMP(bpt, key,                                            \
                         BPT_GET_ENTRY(T, node, k - 1, _key_)) > 0)))         \
	  {                                                                   \
	    *ndi = k;                                                         \
	    return (0);                                                       \
	  }                                                                   \
                                                                              \
      if (BPT_KEYCMP(bpt, key, BPT_GET_ENTRY(T, node, k, _key_)) < 0)         \
	j = k - 1;                                                            \
      else                                                                    \
	i = k + 1;                                                            \
    }                                                                         \
                                                                              \
  if (opts & BPT_OPT_PARTIAL)                                                 \
    {                                                                         \
      if (bpt_last_entry(T, bpt, node, ndi) != 0)                             \
	return (-1);                                                          \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function is used to find an entry in a node.                          \
 *                                                                            \
 * this function either uses a linear or dichotomic search technique          \
 * depending on the flags.                                                    \
 */                                                                           \
                                                                              \
int			bpt_search_entry_##T(t_bpt(T)		*bpt,         \
					     t_bpt_imm(T)	*node,        \
					     t_bpt_key(T)	key,          \
					     t_bpt_ndi(T)	*ndi,         \
					     t_bpt_opts		opts)         \
{                                                                             \
  if (bpt->flags & BPT_FLAG_COLLIDE)                                          \
    return (bpt_dichotomic2_search(T, bpt, node, key, ndi, opts));            \
  else                                                                        \
    return (bpt_dichotomic_search(T, bpt, node, key, ndi, opts));             \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for the leaf entry that theorically contains           \
 * the key.                                                                   \
 */                                                                           \
                                                                              \
int			bpt_search_leaf_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_node(T)	*leaf,        \
					    t_bpt_key(T)	key)          \
{                                                                             \
  t_bpt_imm(T)		child;                                                \
  t_bpt_ndi(T)		ndi;                                                  \
                                                                              \
  if (BPT_GET_HEAD(T, node, type) == BPT_TYPE_LEAF)                           \
    {                                                                         \
      *leaf = node->addr;                                                     \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  if (bpt_search_entry(T, bpt, node, key, &ndi, BPT_OPT_PARTIAL) != 0)        \
    return (-1);                                                              \
                                                                              \
  BPT_LOAD(bpt, &child, BPT_GET_INENTRY(T, node, ndi, _value_));              \
                                                                              \
  if (bpt_search_leaf(T, bpt, &child, leaf, key) != 0)                        \
    {                                                                         \
      BPT_UNLOAD(bpt, &child);                                                \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &child);                                                    \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for a key and returns the address of the node and      \
 * the index of the entry containing this key.                                \
 *                                                                            \
 * be careful not to using this function with collisions because              \
 * the bpt_collide_search() function exists.                                  \
 */                                                                           \
                                                                              \
int			bpt_search_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_entry(T)		*entry)       \
{                                                                             \
  t_bpt_imm(T)		leaf;                                                 \
  t_bpt_imm(T)		root;                                                 \
                                                                              \
  BPT_LOAD(bpt, &root, BPT_GET_ROOT(bpt));                                    \
                                                                              \
  if (bpt_search_leaf(T, bpt, &root, &entry->node, key) != 0)                 \
    {                                                                         \
      bpt_debug(stderr, "[bptd] no leaf present in the tree\n");              \
      BPT_UNLOAD(bpt, &root);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &root);                                                     \
  BPT_LOAD(bpt, &leaf, entry->node);                                          \
                                                                              \
  if (bpt_search_entry(T, bpt, &leaf, key, &entry->ndi,                       \
                       BPT_OPT_PERFECT) != 0)                                 \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot find the entry\n");                    \
      BPT_UNLOAD(bpt, &leaf);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &leaf);                                                     \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function tries to find the next entry which collides with             \
 * the current one specified by the entry variable.                           \
 *                                                                            \
 * first try to find an entry with the same key in the current node. if no    \
 * one exists, look for in the next node because leaves are doubly linked.    \
 *                                                                            \
 * the function returns -1 if there is no other entry in collision.           \
 */                                                                           \
                                                                              \
int			bpt_collide_next_##T(t_bpt(T)		*bpt,         \
					     t_bpt_key(T)	key,          \
					     t_bpt_entry(T)	*entry)       \
{                                                                             \
  t_bpt_imm(T)		node;                                                 \
                                                                              \
  if (bpt_next_entry(T, bpt, *entry, entry, BPT_OPT_TREE) != 0)               \
    return (-1);                                                              \
                                                                              \
  BPT_LOAD(bpt, &node, entry->node);                                          \
                                                                              \
  if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, &node, entry->ndi, _key_), key) != 0)  \
    {                                                                         \
      BPT_UNLOAD(bpt, &node);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &node);                                                     \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function looks for a leaf entry in collision.                         \
 */                                                                           \
                                                                              \
int			bpt_collide_search_##T(t_bpt(T)		*bpt,         \
					       t_bpt_key(T)	key,          \
					       t_bpt_value(T)	value,        \
					       t_bpt_entry(T)	*entry)       \
{                                                                             \
  t_bpt_imm(T)		node;                                                 \
                                                                              \
  if (bpt_search(T, bpt, key, entry) != 0)                                    \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot find the entry in collision\n");       \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  do                                                                          \
    {                                                                         \
      BPT_LOAD(bpt, &node, entry->node);                                      \
                                                                              \
      if ((BPT_KEYCMP(bpt, BPT_GET_LFENTRY(T, &node, entry->ndi, _key_),      \
                      key) == 0) &&                                           \
	  (BPT_VALCMP(bpt, BPT_GET_LFENTRY(T, &node, entry->ndi, _value_),    \
                      value) == 0))                                           \
	{                                                                     \
	  BPT_UNLOAD(bpt, &node);                                             \
	  return (0);                                                         \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
    } while (bpt_collide_next(T, bpt, key, entry) == 0);                      \
                                                                              \
  return (-1);                                                                \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function checks whether a full collision (key:value) exists           \
 * in a leaf.                                                                 \
 */                                                                           \
                                                                              \
int			bpt_check_collide_##T(t_bpt(T)		*bpt,         \
					      t_bpt_imm(T)	*node1,       \
					      t_bpt_key(T)	key,          \
					      t_bpt_value(T)	value)        \
{                                                                             \
  t_bpt_imm(T)		node2;                                                \
  t_bpt_entry(T)	entry;                                                \
                                                                              \
  entry.node = node1->addr;                                                   \
  if (bpt_search_entry(T, bpt, node1, key, &entry.ndi, BPT_OPT_PERFECT) != 0) \
    return (0);                                                               \
                                                                              \
  do                                                                          \
    {                                                                         \
      BPT_LOAD(bpt, &node2, entry.node);                                      \
                                                                              \
      if (bpt->flags & BPT_FLAG_COLLIDE)                                      \
        {                                                                     \
          if ((BPT_KEYCMP(bpt, BPT_GET_LFENTRY(T, &node2, entry.ndi, _key_),  \
                      key) == 0) &&                                           \
  	    (BPT_VALCMP(bpt, BPT_GET_LFENTRY(T, &node2, entry.ndi, _value_),  \
                      value) == 0))                                           \
	  {                                                                   \
	    BPT_UNLOAD(bpt, &node2);                                          \
	    return (-1);                                                      \
	  }                                                                   \
	}                                                                     \
      else                                                                    \
        {                                                                     \
          if (BPT_KEYCMP(bpt, BPT_GET_LFENTRY(T, &node2, entry.ndi, _key_),   \
                         key) == 0)                                           \
	  {                                                                   \
	    BPT_UNLOAD(bpt, &node2);                                          \
	    return (-1);                                                      \
	  }                                                                   \
        }                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &node2);                                                \
    } while (bpt_collide_next(T, bpt, key, &entry) == 0);                     \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function returns the number of used entries in the node.              \
 *                                                                            \
 * this function can only be used if the node is sorted and correctly         \
 * built.                                                                     \
 */                                                                           \
                                                                              \
t_bpt_ndi(T)		bpt_node_size_##T(t_bpt(T)		*bpt,         \
				          t_bpt_imm(T)		*node)        \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		j = nkeys - 1;                                        \
  t_bpt_ndi(T)		i = 0;                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, 0, _key_), bpt->ukey) == 0)      \
    return (0);                                                               \
                                                                              \
  while (i <= j)                                                              \
    {                                                                         \
      t_bpt_ndi(T)	k = (i + j) / 2;                                      \
                                                                              \
      if ((BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_),                  \
                      bpt->ukey) != 0) &&                                     \
	  (((k + 1) > j) ||                                                   \
           (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k + 1, _key_),             \
                       bpt->ukey) == 0)))                                     \
	return (k + 1);                                                       \
                                                                              \
      if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, k, _key_), bpt->ukey) == 0)  \
	j = k - 1;                                                            \
      else                                                                    \
	i = k + 1;                                                            \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function tries to simplify the tree. indeed if the current node       \
 * has no neighbour, then the tree can be reduced and the node becomes the    \
 * root.                                                                      \
 */                                                                           \
                                                                              \
void			bpt_simplify_##T(t_bpt(T)		*bpt,         \
				         t_bpt_imm(T)		*node1,       \
				         t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_node(T)		parent;                                               \
  t_bpt_imm(T)		node2;                                                \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node1, parent), bpt->uaddr) == 0)      \
    return ;                                                                  \
                                                                              \
  if ((BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node1, prv), bpt->uaddr) != 0) ||     \
      (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node1, nxt), bpt->uaddr) != 0))       \
    return ;                                                                  \
                                                                              \
  BPT_LOAD(bpt, &node2, node1->addr);                                         \
                                                                              \
  while (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node2, parent), bpt->uaddr) != 0)  \
    {                                                                         \
      parent = BPT_GET_HEAD(T, &node2, parent);                               \
                                                                              \
      BPT_UNLOAD(bpt, &node2);                                                \
                                                                              \
      BPT_LOAD(bpt, &node2, parent);                                          \
                                                                              \
      BPT_RELEASE(bpt, unused, node2.addr);                                   \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &node2);                                                    \
                                                                              \
  BPT_SET_HEAD(T, node1, parent, bpt->uaddr);                                 \
  BPT_SET_ROOT(bpt, node1->addr);                                             \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function just moves node1's entries into node2 and then               \
 * eliminates the node1 since it is not used any more.                        \
 */                                                                           \
                                                                              \
int			bpt_balancein_1_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_imm(T)		parent;                                               \
  t_bpt_entry(T)	entry;                                                \
                                                                              \
  bpt_insert_head(T, bpt, node1, node2);                                      \
                                                                              \
  BPT_SET_HEAD(T, node2, prv, BPT_GET_HEAD(T, node1, prv));                   \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node2, prv), bpt->uaddr) != 0)         \
    {                                                                         \
      t_bpt_imm(T)	node3;                                                \
                                                                              \
      BPT_LOAD(bpt, &node3, BPT_GET_HEAD(T, node2, prv));                     \
      BPT_SET_HEAD(T, &node3, nxt, node2->addr);                              \
      BPT_UNLOAD(bpt, &node3);                                                \
    }                                                                         \
                                                                              \
  if (BPT_GET_HEAD(T, node2, type) == BPT_TYPE_INTERNAL)                      \
    if (bpt_update(T, bpt, node2, BPT_OPT_PARENT) != 0)                       \
      return (-1);                                                            \
                                                                              \
  BPT_LOAD(bpt, &parent, BPT_GET_HEAD(T, node1, parent));                     \
                                                                              \
  entry.node = parent.addr;                                                   \
  if (bpt_ndi(T, bpt, &parent, node1->addr, &entry.ndi) != 0)                 \
    {                                                                         \
      BPT_UNLOAD(bpt, &parent);                                               \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &parent);                                                   \
                                                                              \
  if (bpt_delete(T, bpt, entry, cbctx, unused) != 0)                          \
    return (-1);                                                              \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                     \
                cb, BPT_CB_MIGRATE);                                          \
  if (BPT_ADDRCMP(bpt, prev.node, node1->addr) == 0)                          \
    {                                                                         \
       BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                \
                     previous.node, node2->addr);                             \
       BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                \
                     previous.ndi, prev.ndi);                                 \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.node, prev.node);                                \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.ndi, prev.ndi);                                  \
    }                                                                         \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                     \
                node, node2->addr);                                           \
                                                                              \
  BPT_RELEASE(bpt, unused, node1->addr);                                      \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function just takes an entry in the next node and inserts it          \
 * in the current node to not have to move the entries.                       \
 */                                                                           \
                                                                              \
int			bpt_balancein_2_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_ndi(T)		ndi1;                                                 \
  t_bpt_ndi(T)		ndi2;                                                 \
                                                                              \
  bpt_insert_sort(T, bpt, node1, bpt->ukey, &ndi1, BPT_OPT_TAIL);             \
                                                                              \
  if (bpt_first_entry(T, bpt, node2, &ndi2) != 0)                             \
    return (-1);                                                              \
                                                                              \
  BPT_COPY_ENTRY(T, node2, node1, ndi2, ndi1);                                \
                                                                              \
  BPT_INIT_ENTRY(T, node2, ndi2);                                             \
  BPT_SET_ENTRY(T, node2, ndi2, _key_, bpt->ukey);                            \
                                                                              \
  bpt_shift_sort(T, bpt, node2);                                              \
                                                                              \
  if (BPT_GET_HEAD(T, node1, type) == BPT_TYPE_INTERNAL)                      \
    if (bpt_update(T, bpt, node1, BPT_OPT_PARENT) != 0)                       \
      return (-1);                                                            \
                                                                              \
  if (bpt_update(T, bpt, node1, BPT_OPT_KEY) != 0)                            \
    return (-1);                                                              \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                cb, BPT_CB_BALANCE);                                          \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                previous.node, prev.node);                                    \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                previous.ndi, prev.ndi);                                      \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                node1, node1->addr);                                          \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                node2, node2->addr);                                          \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function balances node1's entries into node2 and marks                \
 * node1 as unused.                                                           \
 */                                                                           \
                                                                              \
int			bpt_balancein_3_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_ndi(T)		node1sz = bpt_node_size(T, bpt, node1);               \
  t_bpt_imm(T)		parent;                                               \
  t_bpt_entry(T)	entry;                                                \
                                                                              \
  bpt_insert_tail(T, bpt, node2, node1);                                      \
                                                                              \
  BPT_SET_HEAD(T, node1, nxt, BPT_GET_HEAD(T, node2, nxt));                   \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node1, nxt), bpt->uaddr) != 0)         \
    {                                                                         \
      t_bpt_imm(T)	node3;                                                \
                                                                              \
      BPT_LOAD(bpt, &node3, BPT_GET_HEAD(T, node2, nxt));                     \
      BPT_SET_HEAD(T, &node3, prv, node1->addr);                              \
      BPT_UNLOAD(bpt, &node3);                                                \
    }                                                                         \
                                                                              \
  if (BPT_GET_HEAD(T, node2, type) == BPT_TYPE_INTERNAL)                      \
    if (bpt_update(T, bpt, node1, BPT_OPT_PARENT) != 0)                       \
      return (-1);                                                            \
                                                                              \
  if (bpt_update(T, bpt, node1, BPT_OPT_KEY) != 0)                            \
    return (-1);                                                              \
                                                                              \
  BPT_LOAD(bpt, &parent, BPT_GET_HEAD(T, node2, parent));                     \
                                                                              \
  entry.node = BPT_GET_HEAD(T, node2, parent);                                \
  if (bpt_ndi(T, bpt, &parent, node2->addr, &entry.ndi) != 0)                 \
    {                                                                         \
      BPT_UNLOAD(bpt, &parent);                                               \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &parent);                                                   \
                                                                              \
  if (bpt_delete(T, bpt, entry, cbctx, unused) != 0)                          \
    return (-1);                                                              \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                cb, BPT_CB_MIGRATE);                                          \
  if (BPT_ADDRCMP(bpt, prev.node, node2->addr) == 0)                          \
    {                                                                         \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                 \
                    previous.node, node1->addr);                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                 \
                    previous.ndi, node1sz + prev.ndi);                        \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                 \
                    previous.node, prev.node);                                \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                 \
                    previous.ndi, prev.ndi);                                  \
    }                                                                         \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                node, node1->addr);                                           \
                                                                              \
  BPT_RELEASE(bpt, unused, node2->addr);                                      \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function moves one entry of the node2 in node1.                       \
 */                                                                           \
                                                                              \
int			bpt_balancein_4_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node2,       \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_cbctx(T)	*cbctx,       \
					    t_bpt_entry(T)	prev,         \
					    t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_ndi(T)		ndi1;                                                 \
  t_bpt_ndi(T)		ndi2;                                                 \
                                                                              \
  bpt_insert_sort(T, bpt, node2, bpt->ukey, &ndi2, BPT_OPT_HEAD);             \
                                                                              \
  if (bpt_last_entry(T, bpt, node1, &ndi1) != 0)                              \
    return (-1);                                                              \
                                                                              \
  BPT_COPY_ENTRY(T, node1, node2, ndi1, ndi2);                                \
                                                                              \
  BPT_INIT_ENTRY(T, node1, ndi1);                                             \
  BPT_SET_ENTRY(T, node1, ndi1, _key_, bpt->ukey);                            \
                                                                              \
  if (BPT_GET_HEAD(T, node2, type) == BPT_TYPE_INTERNAL)                      \
    if (bpt_update(T, bpt, node2, BPT_OPT_PARENT) != 0)                       \
      return (-1);                                                            \
                                                                              \
  if (bpt_update(T, bpt, node1, BPT_OPT_KEY) != 0)                            \
    return (-1);                                                              \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                     \
                cb, BPT_CB_BALANCE);                                          \
  if ((BPT_ADDRCMP(bpt, prev.node, node1->addr) == 0) &&                      \
      (prev.ndi == ndi1))                                                     \
    {                                                                         \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.node, node2->addr);                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.ndi, ndi2);                                      \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.node, prev.node);                                \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    previous.ndi, prev.ndi + 1);                              \
    }                                                                         \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                     \
                node1, node1->addr);                                          \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                     \
                node2, node2->addr);                                          \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function deletes an entry in a given node and re-balances             \
 * entries if necessary.                                                      \
 *                                                                            \
 * if a balancing has to be done, the function operates according             \
 * to four cases:                                                             \
 *                                                                            \
 * 1) the next node is large enough to receive the entries of the node1.      \
 * 2) the next node is not large enough so we balance the two nodes           \
 *    taking a node2's entry to move it into node1.                           \
 * 3) the previous node is large enough to receive the entries of the node1.  \
 * 4) the previous node is not large enough so we balance the two nodes       \
 *    taking a node2's entry to move it in the node1.                         \
 */                                                                           \
                                                                              \
int			bpt_delete_##T(t_bpt(T)			*bpt,         \
				       t_bpt_entry(T)		entry,        \
				       t_bpt_cbctx(T)		*cbctx,       \
				       t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_imm(T)		node1;                                                \
  t_bpt_imm(T)		node2;                                                \
  t_bpt_ndi(T)		nkeys;                                                \
  t_bpt_entry(T)	prev;                                                 \
                                                                              \
  if (bpt_prev_entry(T, bpt, entry, &prev, BPT_OPT_TREE) != 0)                \
    {                                                                         \
      prev.node = bpt->uaddr;                                                 \
      prev.ndi = 0;                                                           \
    }                                                                         \
                                                                              \
  BPT_LOAD(bpt, &node1, entry.node);                                          \
  nkeys = BPT_NKEYS(T, bpt, &node1);                                          \
                                                                              \
  BPT_INIT_ENTRY(T, &node1, entry.ndi);                                       \
  BPT_SET_ENTRY(T, &node1, entry.ndi, _key_, bpt->ukey);                      \
                                                                              \
  bpt_shift_sort(T, bpt, &node1);                                             \
                                                                              \
  if ((BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node1, parent), bpt->uaddr) == 0) || \
      (bpt_node_size(T, bpt, &node1) >= BPT_BALANCING(T, bpt, &node1)))       \
    {                                                                         \
      if (bpt_update(T, bpt, &node1, BPT_OPT_KEY) != 0)                       \
        {                                                                     \
          bpt_debug(stderr, "[bptd] cannot update the node\n");               \
          BPT_UNLOAD(bpt, &node1);                                            \
          return (-1);                                                        \
        }                                                                     \
                                                                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type), cbctx,                \
                    cb, BPT_CB_DELETE);                                       \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type), cbctx,                \
                    previous.node, prev.node);                                \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type), cbctx,                \
                    previous.ndi, prev.ndi);                                  \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type), cbctx,                \
                    node, node1.addr);                                        \
                                                                              \
      BPT_UNLOAD(bpt, &node1);                                                \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node1, nxt), bpt->uaddr) != 0)        \
    {                                                                         \
      BPT_LOAD(bpt, &node2, BPT_GET_HEAD(T, &node1, nxt));                    \
                                                                              \
      if ((bpt_node_size(T, bpt, &node2) +                                    \
           bpt_node_size(T, bpt, &node1)) <= nkeys)                           \
	{                                                                     \
	  if (bpt_balancein_1(T, bpt, &node1, &node2, cbctx,                  \
                              prev, unused) != 0)                             \
	    {                                                                 \
              bpt_debug(stderr, "[bptd] cannot balance entries in the "       \
                        "second node\n");                                     \
	      BPT_UNLOAD(bpt, &node2);                                        \
	      BPT_UNLOAD(bpt, &node1);                                        \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  bpt_simplify(T, bpt, &node2, unused);                               \
                                                                              \
	  BPT_UNLOAD(bpt, &node2);                                            \
	  BPT_UNLOAD(bpt, &node1);                                            \
	  return (0);                                                         \
	}                                                                     \
      else                                                                    \
	{                                                                     \
	  if (bpt_balancein_2(T, bpt, &node1, &node2, cbctx,                  \
                              prev, unused) != 0)                             \
	    {                                                                 \
              bpt_debug(stderr, "[bptd] cannot balance one entry in "         \
                                "the node\n");                                \
	      BPT_UNLOAD(bpt, &node2);                                        \
	      BPT_UNLOAD(bpt, &node1);                                        \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  BPT_UNLOAD(bpt, &node2);                                            \
	  BPT_UNLOAD(bpt, &node1);                                            \
	  return (0);                                                         \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &node2);                                                \
    }                                                                         \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, &node1, prv), bpt->uaddr) != 0)        \
    {                                                                         \
      BPT_LOAD(bpt, &node2, BPT_GET_HEAD(T, &node1, prv));                    \
                                                                              \
      if ((bpt_node_size(T, bpt, &node2) +                                    \
           bpt_node_size(T, bpt, &node1)) <= nkeys)                           \
	{                                                                     \
	  if (bpt_balancein_3(T, bpt, &node1, &node2, cbctx,                  \
                              prev, unused) != 0)                             \
	    {                                                                 \
              bpt_debug(stderr, "[bptd] cannot balance entries in the "       \
                                "second node\n");                             \
	      BPT_UNLOAD(bpt, &node2);                                        \
	      BPT_UNLOAD(bpt, &node1);                                        \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  bpt_simplify(T, bpt, &node2, unused);                               \
                                                                              \
	  BPT_UNLOAD(bpt, &node2);                                            \
	  BPT_UNLOAD(bpt, &node1);                                            \
	  return (0);                                                         \
	}                                                                     \
      else                                                                    \
	{                                                                     \
	  if (bpt_balancein_4(T, bpt, &node1, &node2, cbctx,                  \
                              prev, unused) != 0)                             \
	    {                                                                 \
              bpt_debug(stderr, "[bptd] cannot balance one entry in "         \
                                "the node\n");                                \
	      BPT_UNLOAD(bpt, &node2);                                        \
	      BPT_UNLOAD(bpt, &node1);                                        \
	      return (-1);                                                    \
	    }                                                                 \
                                                                              \
	  BPT_UNLOAD(bpt, &node2);                                            \
	  BPT_UNLOAD(bpt, &node1);                                            \
	  return (0);                                                         \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &node2);                                                \
    }                                                                         \
                                                                              \
  if (bpt_update(T, bpt, &node1, BPT_OPT_KEY) != 0)                           \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot update the node\n");                   \
      BPT_UNLOAD(bpt, &node1);                                                \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  bpt_simplify(T, bpt, &node1, unused);                                       \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type),                           \
                cbctx, cb, BPT_CB_DELETE);                                    \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type),                           \
                cbctx, previous.node, prev.node);                             \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type),                           \
                cbctx, previous.ndi, prev.ndi);                               \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node1, type),                           \
                cbctx, node, node1.addr);                                     \
                                                                              \
  BPT_UNLOAD(bpt, &node1);                                                    \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function removes a leaf entry.                                        \
 *                                                                            \
 * this function does not take care about collisions assuming there is no     \
 * one. like bpt_modify(), if the programmer wants to remove a specific       \
 * entry that possibly collide with others, he has to use the                 \
 * bpt_collide_remove() function.                                             \
 *                                                                            \
 * the function also fills the _unused_ array with the addresses of the now   \
 * unused blocks.                                                             \
 */                                                                           \
                                                                              \
int			bpt_remove_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_cbctx(T)	cbctx;                                                \
  t_bpt_entry(T)	entry;                                                \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_REMOVE) != 0)                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (bpt_search(T, bpt, key, &entry) != 0)                                   \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot find the entry looked for\n");         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_INIT_CBCTX(bpt, &cbctx);                                                \
                                                                              \
  if (bpt_delete(T, bpt, entry, &cbctx, unused) != 0)                         \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot delete the leaf entry\n");             \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_CALLBACK(bpt, &cbctx);                                                  \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function is similar to the bpt_remove() one.                          \
 *                                                                            \
 * the singular difference is that the function takes an entry descriptor     \
 * as parameter. by this, this function is able to remove an entry that       \
 * collide with others.                                                       \
 */                                                                           \
                                                                              \
int			bpt_collide_remove_##T(t_bpt(T)		*bpt,         \
					       t_bpt_entry(T)	entry,        \
					       t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_cbctx(T)	cbctx;                                                \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_REMOVE) != 0)                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_INIT_CBCTX(bpt, &cbctx);                                                \
                                                                              \
  if (bpt_delete(T, bpt, entry, &cbctx, unused) != 0)                         \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot delete the leaf entry\n");             \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_CALLBACK(bpt, &cbctx);                                                  \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function modifies an entry. this entry may be moved due to the        \
 * key modification.                                                          \
 *                                                                            \
 * this function supposes that there is no collision. if the tree accepts     \
 * collisions, we advice you not to use this function. instead you must       \
 * implement your own version or use bpt_collide_modify().                    \
 *                                                                            \
 * the _unused_ variable must contain free blocks that bpt can use            \
 * for splitting.                                                             \
 *                                                                            \
 * this restriction may seem abusive, so *only* if the programmer knows       \
 * that the new leaf entry has the same key, this function can be called      \
 * with a NULL unused variable because it will never be used.                 \
 *                                                                            \
 * to know what the _unused_ variable is supposed to contain, take a look     \
 * at the bpt_add() function.                                                 \
 */                                                                           \
                                                                              \
int			bpt_modify_##T(t_bpt(T)			*bpt,         \
				       t_bpt_key(T)		key,          \
				       t_bpt_lfentry(T)		*lfentry,     \
				       t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_cbctx(T)	cbctx;                                                \
  t_bpt_entry(T)	entry;                                                \
  t_bpt_imm(T)		node;                                                 \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_MODIFY) != 0)                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (BPT_KEYCMP(bpt, lfentry->_key_, bpt->ukey) == 0)                        \
    {                                                                         \
      bpt_debug(stderr, "[bptd] the leaf entry to update has a "              \
                        "non-valid key\n");                                   \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (bpt_search(T, bpt, key, &entry) != 0)                                   \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot find the entry\n");                    \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_LOAD(bpt, &node, entry.node);                                           \
                                                                              \
  BPT_INIT_CBCTX(bpt, &cbctx);                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, lfentry->_key_,                                         \
                 BPT_GET_LFENTRY(T, &node, entry.ndi, _key_)) == 0)           \
    {                                                                         \
      BPT_IMPORT_LFENTRY(T, &node, entry.ndi, lfentry);                       \
                                                                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    cb, BPT_CB_MODIFY);                                       \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    current.node, node.addr);                                 \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    current.ndi, entry.ndi);                                  \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    node, node.addr);                                         \
                                                                              \
      BPT_CALLBACK(bpt, &cbctx);                                              \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &node);                                                     \
                                                                              \
  if (bpt_remove(T, bpt, key, unused) != 0)                                   \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot remove the entry\n");                  \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (bpt_add(T, bpt, lfentry, unused) != 0)                                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot add the entry\n");                     \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function is similar to the bpt_modify() function except               \
 * that it takes a entry descriptor to specify a precise entry to modify.     \
 *                                                                            \
 * this function is adapted to the trees that manage collisions.              \
 */                                                                           \
                                                                              \
int			bpt_collide_modify_##T(t_bpt(T)		*bpt,         \
					       t_bpt_entry(T)	entry,        \
					       t_bpt_lfentry(T)	*lfentry,     \
					       t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_cbctx(T)	cbctx;                                                \
  t_bpt_imm(T)		node;                                                 \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_MODIFY) != 0)                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (BPT_KEYCMP(bpt, lfentry->_key_, bpt->ukey) == 0)                        \
    {                                                                         \
      bpt_debug(stderr, "[bptd] the leaf entry to update has a "              \
                        "non-valid key\n");                                   \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_LOAD(bpt, &node, entry.node);                                           \
                                                                              \
  BPT_INIT_CBCTX(bpt, &cbctx);                                                \
                                                                              \
  if (BPT_KEYCMP(bpt, lfentry->_key_,                                         \
                 BPT_GET_LFENTRY(T, &node, entry.ndi, _key_)) == 0)           \
    {                                                                         \
      BPT_IMPORT_LFENTRY(T, &node, entry.ndi, lfentry);                       \
                                                                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    cb, BPT_CB_MODIFY);                                       \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    current.node, node.addr);                                 \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    current.ndi, entry.ndi);                                  \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, &node, type), &cbctx,                \
                    node, node.addr);                                         \
                                                                              \
      BPT_CALLBACK(bpt, &cbctx);                                              \
                                                                              \
      BPT_UNLOAD(bpt, &node);                                                 \
                                                                              \
      return (0);                                                             \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &node);                                                     \
                                                                              \
  if (bpt_collide_remove(T, bpt, entry, unused) != 0)                         \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot remove a collision\n");                \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (bpt_add(T, bpt, lfentry, unused) != 0)                                  \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot add an entry\n");                      \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function inserts node1's entries at the head of the node2.            \
 */                                                                           \
                                                                              \
void			bpt_insert_head_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2)       \
{                                                                             \
  t_bpt_ndi(T)		node1sz = bpt_node_size(T, bpt, node1);               \
  t_bpt_ndi(T)		node2sz = bpt_node_size(T, bpt, node2);               \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = node2sz - 1; i >= 0; i--)                                          \
    BPT_COPY_ENTRY(T, node2, node2, i, node1sz + i);                          \
  for (i = 0; i < node1sz; i++)                                               \
    BPT_COPY_ENTRY(T, node1, node2, i, i);                                    \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function inserts node1's entries at the tail of the node2.            \
 */                                                                           \
                                                                              \
void			bpt_insert_tail_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node1,       \
					    t_bpt_imm(T)	*node2)       \
{                                                                             \
  t_bpt_ndi(T)		node2sz = bpt_node_size(T, bpt, node2);               \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node1);                     \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  for (i = 0;                                                                 \
       (i < nkeys) && (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node1, i, _key_),     \
				  bpt->ukey) != 0);                           \
       i++)                                                                   \
    BPT_COPY_ENTRY(T, node1, node2, i, node2sz + i);                          \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function shifts entries to place unused entries at the end of         \
 * the node.                                                                  \
 */                                                                           \
                                                                              \
void			bpt_shift_sort_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node)        \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_entry(T)	current;                                              \
                                                                              \
  for (current.node = node->addr, current.ndi = 0;                            \
       current.ndi < nkeys; current.ndi++)                                    \
    if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, current.ndi, _key_),           \
	           bpt->ukey) == 0)                                           \
      {                                                                       \
	t_bpt_entry(T)	next;                                                 \
                                                                              \
	if (bpt_next_entry(T, bpt, current, &next, BPT_OPT_NODE) != 0)        \
          break;                                                              \
                                                                              \
        BPT_SWAP_ENTRIES(T, node, current.ndi, next.ndi);                     \
      }                                                                       \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function inserts and sorts an entry.                                  \
 */                                                                           \
                                                                              \
void			bpt_insert_sort_##T(t_bpt(T)		*bpt,         \
					    t_bpt_imm(T)	*node,        \
					    t_bpt_key(T)	key,          \
					    t_bpt_ndi(T)	*ndi,         \
					    t_bpt_opts		opts)         \
{                                                                             \
  t_bpt_ndi(T)		nodesz = bpt_node_size(T, bpt, node);                 \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  if (opts & BPT_OPT_HEAD)                                                    \
    {                                                                         \
      for (*ndi = 0, i = nodesz - 1; i >= 0; i--)                             \
	BPT_COPY_ENTRY(T, node, node, i, i + 1);                              \
    }                                                                         \
  else if (opts & BPT_OPT_TAIL)                                               \
    {                                                                         \
      *ndi = nodesz;                                                          \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      for (*ndi = 0;                                                          \
           (*ndi < nkeys) &&                                                  \
           (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, *ndi, _key_),              \
                       bpt->ukey) != 0);                                      \
           (*ndi)++)                                                          \
	if (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, *ndi, _key_), key) > 0)    \
	  break;                                                              \
                                                                              \
      if (nodesz > *ndi)                                                      \
	for (i = nodesz - 1; i >= *ndi; i--)                                  \
	  BPT_COPY_ENTRY(T, node, node, i, i + 1);                            \
    }                                                                         \
                                                                              \
  BPT_INIT_ENTRY(T, node, *ndi);                                              \
  BPT_SET_ENTRY(T, node, *ndi, _key_, bpt->ukey);                             \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function creates a new root node and then inserts two entries         \
 * in it for the two children node1 and node2.                                \
 */                                                                           \
                                                                              \
void			bpt_new_root_##T(t_bpt(T)		*bpt,         \
				         t_bpt_imm(T)		*node1,       \
				         t_bpt_imm(T)		*node2,       \
				         t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_inentry(T)	inentry;                                              \
  t_bpt_imm(T)		root;                                                 \
  t_bpt_addr(T)		blk;                                                  \
  t_bpt_ndi(T)		ndi;                                                  \
                                                                              \
  BPT_RESERVE(bpt, unused, blk);                                              \
  BPT_LOAD(bpt, &root, blk);                                                  \
                                                                              \
  bpt_make_node(T, bpt, &root, BPT_TYPE_INTERNAL);                            \
                                                                              \
  memset(&inentry, 0x0, sizeof(t_bpt_inentry(T)));                            \
  bpt_key(T, bpt, node1, &inentry._key_);                                     \
  inentry._value_ = node1->addr;                                              \
                                                                              \
  bpt_insert_sort(T, bpt, &root, inentry._key_, &ndi, BPT_OPT_ZERO);          \
                                                                              \
  BPT_IMPORT_INENTRY(T, &root, ndi, &inentry);                                \
                                                                              \
  memset(&inentry, 0x0, sizeof(t_bpt_inentry(T)));                            \
  bpt_key(T, bpt, node2, &inentry._key_);                                     \
  inentry._value_ = node2->addr;                                              \
                                                                              \
  bpt_insert_sort(T, bpt, &root, inentry._key_, &ndi, BPT_OPT_ZERO);          \
                                                                              \
  BPT_IMPORT_INENTRY(T, &root, ndi, &inentry);                                \
                                                                              \
  BPT_SET_ROOT(bpt, root.addr);                                               \
  BPT_SET_HEAD(T, node1, parent, root.addr);                                  \
  BPT_SET_HEAD(T, node2, parent, root.addr);                                  \
                                                                              \
  bpt->height++;                                                              \
                                                                              \
  BPT_UNLOAD(bpt, &root);                                                     \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function, used by bpt_split_node() re-balances the                    \
 * entries of node1 in the two nodes node1 and node2.                         \
 */                                                                           \
                                                                              \
int			bpt_balanceout_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node1,       \
				           t_bpt_imm(T)		*node2,       \
				           void			*entry,       \
				           t_bpt_cbctx(T)	*cbctx)       \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node1);                     \
  t_bpt_ndi(T)		i;                                                    \
  t_bpt_ndi(T)		j;                                                    \
                                                                              \
  BPT_COPY_NODE(bpt, node1->buf, node2->buf);                                 \
  bpt_reinit_entries(T, bpt, node1);                                          \
                                                                              \
  for (i = 0, j = 0; i < ((nkeys + 1) / 2); i++)                              \
    {                                                                         \
      if ((entry != NULL) &&                                                  \
	  (BPT_KEYCMP(bpt, BPT_GET_THIS_ENTRY(T, BPT_GET_HEAD(T, node1, type),\
                                              entry, _key_),                  \
                      BPT_GET_ENTRY(T, node2, j, _key_)) < 0))                \
	{                                                                     \
	  BPT_IMPORT_ENTRY(T, node1, i, entry);                               \
                                                                              \
          BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,             \
                        current.node, node1->addr);                           \
          BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,             \
                        current.ndi, i);                                      \
                                                                              \
	  entry = NULL;                                                       \
	  continue;                                                           \
	}                                                                     \
                                                                              \
      BPT_COPY_ENTRY(T, node2, node1, j, i);                                  \
      BPT_INIT_ENTRY(T, node2, j);                                            \
      BPT_SET_ENTRY(T, node2, j, _key_, bpt->ukey);                           \
      j++;                                                                    \
    }                                                                         \
                                                                              \
  bpt_shift_sort(T, bpt, node2);                                              \
                                                                              \
  if (entry != NULL)                                                          \
    {                                                                         \
      bpt_insert_sort(T, bpt, node2,                                          \
		      BPT_GET_THIS_ENTRY(T, BPT_GET_HEAD(T, node1, type),     \
                                         entry, _key_),                       \
		      &i, BPT_OPT_ZERO);                                      \
      BPT_IMPORT_ENTRY(T, node2, i, entry);                                   \
                                                                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    current.node, node2->addr);                               \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node2, type), cbctx,                 \
                    current.ndi, i);                                          \
    }                                                                         \
                                                                              \
  BPT_SET_HEAD(T, node2, prv, node1->addr);                                   \
  BPT_SET_HEAD(T, node2, nxt, BPT_GET_HEAD(T, node1, nxt));                   \
  BPT_SET_HEAD(T, node1, nxt, node2->addr);                                   \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node2, nxt), bpt->uaddr) != 0)         \
    {                                                                         \
      t_bpt_imm(T)	node3;                                                \
                                                                              \
      BPT_LOAD(bpt, &node3, BPT_GET_HEAD(T, node2, nxt));                     \
      BPT_SET_HEAD(T, &node3, prv, node2->addr);                              \
      BPT_UNLOAD(bpt, &node3);                                                \
    }                                                                         \
                                                                              \
  if (BPT_GET_HEAD(T, node1, type) == BPT_TYPE_INTERNAL)                      \
    {                                                                         \
      if (bpt_update(T, bpt, node1, BPT_OPT_PARENT) != 0)                     \
	return (-1);                                                          \
                                                                              \
      if (bpt_update(T, bpt, node2, BPT_OPT_PARENT) != 0)                     \
	return (-1);                                                          \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function splits a node so creates one new node (using an _unused_     \
 * address) and re-organizes the entries. if splitting the root node, a new   \
 * root node has to be created.                                               \
 *                                                                            \
 * the function also inserts the specified entry.                             \
 *                                                                            \
 * finally the function fills the entry descriptor that indicates the         \
 * location of the inserted entry                                             \
 */                                                                           \
                                                                              \
int			bpt_split_node_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node1,       \
				           void			*entry,       \
				           t_bpt_cbctx(T)	*cbctx,       \
				           t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_imm(T)		node2;                                                \
  t_bpt_addr(T)		blk;                                                  \
                                                                              \
  BPT_RESERVE(bpt, unused, blk);                                              \
  BPT_LOAD(bpt, &node2, blk);                                                 \
                                                                              \
  bpt_make_node(T, bpt, &node2, BPT_GET_HEAD(T, node1, type));                \
  BPT_SET_HEAD(T, node1, parent, BPT_GET_HEAD(T, node1, parent));             \
                                                                              \
  if (bpt_balanceout(T, bpt, node1, &node2, entry, cbctx) != 0)               \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot balance entries between two nodes\n"); \
      BPT_UNLOAD(bpt, &node2);                                                \
      BPT_RELEASE(bpt, unused, blk);                                          \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (BPT_ADDRCMP(bpt, BPT_GET_HEAD(T, node1, parent), bpt->uaddr) == 0)      \
    {                                                                         \
      bpt_new_root(T, bpt, node1, &node2, unused);                            \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      t_bpt_inentry(T)	inentry;                                              \
      t_bpt_imm(T)	parent;                                               \
                                                                              \
      if (bpt_update(T, bpt, node1, BPT_OPT_KEY) != 0)                        \
        {                                                                     \
          BPT_UNLOAD(bpt, &node2);                                            \
          BPT_RELEASE(bpt, unused, blk);                                      \
          return (-1);                                                        \
        }                                                                     \
                                                                              \
      memset(&inentry, 0x0, sizeof(t_bpt_inentry(T)));                        \
      bpt_key(T, bpt, &node2, &inentry._key_);                                \
      inentry._value_ = node2.addr;                                           \
                                                                              \
      BPT_LOAD(bpt, &parent, BPT_GET_HEAD(T, &node2, parent));                \
                                                                              \
      if (bpt_insert(T, bpt, &parent, &inentry, cbctx, unused) != 0)          \
	{                                                                     \
          bpt_debug(stderr, "[bptd] cannot insert the entry in the "          \
                            "parent node\n");                                 \
	  BPT_UNLOAD(bpt, &parent);                                           \
	  BPT_UNLOAD(bpt, &node2);                                            \
	  BPT_RELEASE(bpt, unused, blk);                                      \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &parent);                                               \
    }                                                                         \
                                                                              \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                cb, BPT_CB_SPLIT);                                            \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                node1, node1->addr);                                          \
  BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node1, type), cbctx,                     \
                node2, node2.addr);                                           \
                                                                              \
  BPT_UNLOAD(bpt, &node2);                                                    \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function inserts the entry in the specified node.                     \
 *                                                                            \
 * the node may be splitted and the entries re-balanced between the two       \
 * nodes.                                                                     \
 *                                                                            \
 * the _unused_ variable contains _unused_ blocks, one for each level         \
 * of the tree because at each level a node can be splitted and one           \
 * for the possible creation of a new root node.                              \
 */                                                                           \
                                                                              \
int			bpt_insert_##T(t_bpt(T)			*bpt,         \
				       t_bpt_imm(T)		*node,        \
				       void			*entry,       \
				       t_bpt_cbctx(T)		*cbctx,       \
				       t_bpt_unused(T)		*unused)      \
{                                                                             \
  if (bpt_node_size(T, bpt, node) < BPT_NKEYS(T, bpt, node))                  \
    {                                                                         \
      t_bpt_ndi(T)	ndi;                                                  \
                                                                              \
      bpt_insert_sort(T, bpt, node,                                           \
		      BPT_GET_THIS_ENTRY(T, BPT_GET_HEAD(T, node, type),      \
                                         entry, _key_),                       \
		      &ndi, BPT_OPT_ZERO);                                    \
                                                                              \
      BPT_IMPORT_ENTRY(T, node, ndi, entry);                                  \
                                                                              \
      if (bpt_update(T, bpt, node, BPT_OPT_KEY) != 0)                         \
	{                                                                     \
          bpt_debug(stderr, "[bptd] cannot update the tree\n");               \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node, type), cbctx,                  \
                    cb, BPT_CB_INSERT);                                       \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node, type), cbctx,                  \
                    current.node, node->addr);                                \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node, type), cbctx,                  \
                    current.ndi, ndi);                                        \
      BPT_SET_CBCTX(bpt, BPT_GET_HEAD(T, node, type), cbctx,                  \
                    node, node->addr);                                        \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      if (bpt_split_node(T, bpt, node, entry, cbctx, unused) != 0)            \
	{                                                                     \
          bpt_debug(stderr, "[bptd] cannot split the node\n");                \
	  return (-1);                                                        \
	}                                                                     \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function adds the lfentry in a leaf of the tree.                      \
 *                                                                            \
 * the tree may grow and split some nodes to insert the new entry.            \
 *                                                                            \
 * the variable _unused_ contains N+1 _unused_ block addresses where N is     \
 * the tree's height. the '+1' designs the possible creation of a new         \
 * root node.                                                                 \
 *                                                                            \
 * if the programmer knows that the insertion of the new entry will           \
 * only split M node - where M is lower than N and possibly zero - the        \
 * _unused_ variable can be built in consequences.                            \
 */                                                                           \
                                                                              \
int			bpt_add_##T(t_bpt(T)			*bpt,         \
				    t_bpt_lfentry(T)		*lfentry,     \
				    t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_cbctx(T)	cbctx;                                                \
  t_bpt_node(T)		addr;                                                 \
  t_bpt_imm(T)		leaf;                                                 \
  t_bpt_imm(T)		root;                                                 \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_ADD) != 0)                     \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (BPT_KEYCMP(bpt, lfentry->_key_, bpt->ukey) == 0)                        \
    {                                                                         \
      bpt_debug(stderr, "[bptd] the leaf entry to insert has a "              \
                        "non-valid key\n");                                   \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_LOAD(bpt, &root, BPT_GET_ROOT(bpt));                                    \
                                                                              \
  if (bpt_search_leaf(T, bpt, &root, &addr, lfentry->_key_) != 0)             \
    {                                                                         \
      bpt_debug(stderr, "[bptd] no leaf present in the tree\n");              \
      BPT_UNLOAD(bpt, &root);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &root);                                                     \
  BPT_LOAD(bpt, &leaf, addr);                                                 \
                                                                              \
  if (bpt_check_collide(T, bpt, &leaf, lfentry->_key_, lfentry->_value_) != 0)\
    {                                                                         \
      bpt_debug(stderr, "[bptd] this entry collide with an other\n");         \
      BPT_UNLOAD(bpt, &leaf);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_INIT_CBCTX(bpt, &cbctx);                                                \
                                                                              \
  if (bpt_insert(T, bpt, &leaf, lfentry, &cbctx, unused) != 0)                \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot insert the leaf entry\n");             \
      BPT_UNLOAD(bpt, &leaf);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_CALLBACK(bpt, &cbctx);                                                  \
                                                                              \
  BPT_UNLOAD(bpt, &leaf);                                                     \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function initializes the core structure and build the tree's          \
 * root node.                                                                 \
 */                                                                           \
                                                                              \
int			bpt_init_##T(t_bpt(T)			*bpt,         \
				     t_bpt_ndi(T)		nodesz,       \
				     t_bpt_addr(T)		uaddr,        \
				     t_bpt_key(T)		ukey,         \
				     t_bpt_value(T)		uval,         \
				     t_bpt_flags		flags,        \
				     BPT_BALANCING_T		balancing,    \
				     t_bpt_load_fn(T)		load,         \
				     t_bpt_unload_fn(T)		unload,       \
				     t_bpt_addrcmp_fn(T)	addrcmp,      \
				     t_bpt_keycmp_fn(T)		keycmp,       \
				     t_bpt_valcmp_fn(T)		valcmp,       \
				     t_bpt_cb_fn(T)		callback,     \
				     void			*data,        \
				     t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_imm(T)		root;                                                 \
  t_bpt_addr(T)		addr;                                                 \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_INIT) != 0)                    \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  bpt->nodesz = nodesz;                                                       \
  bpt->ukey = ukey;                                                           \
  bpt->uaddr = uaddr;                                                         \
  bpt->uval = uval;                                                           \
  bpt->nodes = 0;                                                             \
  bpt->flags = flags;                                                         \
                                                                              \
  if (bpt->nodesz < sizeof(t_bpt_head(T)))                                    \
    {                                                                         \
      bpt_debug(stderr, "[bptd] node size less than header size\n");          \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  bpt->nikeys = (bpt->nodesz - sizeof(t_bpt_head(T))) /                       \
    sizeof(t_bpt_inentry(T));                                                 \
  bpt->nlkeys = (bpt->nodesz - sizeof(t_bpt_head(T))) /                       \
    sizeof(t_bpt_lfentry(T));                                                 \
  bpt->height = BPT_INIT_HEIGHT;                                              \
                                                                              \
  if ((balancing <= 0) || (balancing >= 100))                                 \
    {                                                                         \
      bpt_debug(stderr, "[bptd] the balancing percentage is incorrect\n");    \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  bpt->ibalancing = (bpt->nikeys * balancing % 100) == 0 ?                    \
                       bpt->nikeys * balancing / 100 :                        \
                       bpt->nikeys * balancing / 100 + 1;                     \
  bpt->lbalancing = (bpt->nlkeys * balancing % 100) == 0 ?                    \
                       bpt->nlkeys * balancing / 100 :                        \
                       bpt->nlkeys * balancing / 100 + 1;                     \
                                                                              \
  bpt->data = data;                                                           \
                                                                              \
  if (bpt->nikeys < BPT_INIT_NIKEYS)                                          \
    {                                                                         \
      bpt_debug(stderr, "[bptd] number of keys per internal node too low\n"); \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  if (bpt->nlkeys < BPT_INIT_NLKEYS)                                          \
    {                                                                         \
      bpt_debug(stderr, "[bptd] number of keys per leaf node too low\n");     \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_RESERVE(bpt, unused, addr);                                             \
  BPT_SET_ROOT(bpt, addr);                                                    \
                                                                              \
  bpt->load = load;                                                           \
  bpt->unload = unload;                                                       \
  bpt->addrcmp = addrcmp;                                                     \
  bpt->keycmp = keycmp;                                                       \
  bpt->valcmp = valcmp;                                                       \
  bpt->callback = callback;                                                   \
                                                                              \
  BPT_LOAD(bpt, &root, BPT_GET_ROOT(bpt));                                    \
                                                                              \
  bpt_make_node(T, bpt, &root, BPT_TYPE_LEAF);                                \
                                                                              \
  BPT_UNLOAD(bpt, &root);                                                     \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function cleans the node marking itself as unused.                    \
 */                                                                           \
                                                                              \
int			bpt_clean_node_##T(t_bpt(T)		*bpt,         \
				           t_bpt_imm(T)		*node,        \
				           t_bpt_unused(T)	*unused)      \
{                                                                             \
  t_bpt_ndi(T)		nkeys = BPT_NKEYS(T, bpt, node);                      \
  t_bpt_imm(T)		child;                                                \
  t_bpt_ndi(T)		i;                                                    \
                                                                              \
  BPT_RELEASE(bpt, unused, node->addr);                                       \
                                                                              \
  if (BPT_GET_HEAD(T, node, type) == BPT_TYPE_LEAF)                           \
    return (0);                                                               \
                                                                              \
  for (i = 0; (i < nkeys) &&                                                  \
              (BPT_KEYCMP(bpt, BPT_GET_ENTRY(T, node, i, _key_),              \
                          bpt->ukey) != 0);                                   \
       i++)                                                                   \
    {                                                                         \
      BPT_LOAD(bpt, &child, BPT_GET_INENTRY(T, node, i, _value_));            \
                                                                              \
      if (bpt_clean_node(T, bpt, &child, unused) != 0)                        \
	{                                                                     \
	  BPT_UNLOAD(bpt, &child);                                            \
	  return (-1);                                                        \
	}                                                                     \
                                                                              \
      BPT_UNLOAD(bpt, &child);                                                \
    }                                                                         \
                                                                              \
  return (0);                                                                 \
}                                                                             \
                                                                              \
/*                                                                            \
 * this function cleans the tree, destroying each node.                       \
 *                                                                            \
 * the _unused_ variable is finally filled with the addresses of the blocks   \
 * composing the tree.                                                        \
 */                                                                           \
                                                                              \
int			bpt_clean_##T(t_bpt(T)			*bpt,         \
				      t_bpt_unused(T)		*unused)      \
{                                                                             \
  t_bpt_imm(T)		root;                                                 \
                                                                              \
  if (bpt_check_unused(T, bpt, unused, BPT_OPT_CLEAN) != 0)                   \
    {                                                                         \
      bpt_debug(stderr, "[bptd] bad unused array\n");                         \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_LOAD(bpt, &root, BPT_GET_ROOT(bpt));                                    \
                                                                              \
  if (bpt_clean_node(T, bpt, &root, unused) != 0)                             \
    {                                                                         \
      bpt_debug(stderr, "[bptd] cannot clean the nodes\n");                   \
      BPT_UNLOAD(bpt, &root);                                                 \
      return (-1);                                                            \
    }                                                                         \
                                                                              \
  BPT_UNLOAD(bpt, &root);                                                     \
                                                                              \
  return (0);                                                                 \
}

/*
 * ---------- meta-macro ------------------------------------------------------
 */

#define		bpt_make(T, _t_bpt_ndi_, _t_bpt_uni_, _t_bpt_nodes_,          \
                         _t_bpt_height_, _t_bpt_addr_, _t_bpt_key_,           \
                         _t_bpt_value_, _t_bpt_inentry_, _t_bpt_lfentry_,     \
                         _key_, _value_)                                      \
  bpt_make_types(T, _t_bpt_ndi_, _t_bpt_uni_, _t_bpt_nodes_, _t_bpt_height_,  \
                 _t_bpt_addr_, _t_bpt_key_, _t_bpt_value_, _t_bpt_inentry_,   \
                 _t_bpt_lfentry_)                                             \
  bpt_make_protos(T)                                                          \
  bpt_make_functions(T, _key_, _value_)

#endif
