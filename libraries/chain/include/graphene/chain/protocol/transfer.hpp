/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Any modified source or binaries are used only with the BitShares network.
 *
 * 2. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>

namespace graphene { namespace chain {

   /**
    * @ingroup operations
    *
    * @brief Transfers an amount of one asset from one account to another
    *
    *  Fees are paid by the "from" account
    *
    *  @pre amount.amount > 0
    *  @pre fee.amount >= 0
    *  @pre from != to
    *  @post from account's balance will be reduced by fee and amount
    *  @post to account's balance will be increased by amount
    *  @return n/a
    */
   struct transfer_operation : public base_operation
   {
      struct fee_parameters_type {
         uint64_t fee             =  20 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte =  10 * GRAPHENE_BLOCKCHAIN_PRECISION; /// only required for large memos.
      };

      asset            fee;
      /// Account to transfer asset from
      account_id_type  from;
      /// Account to transfer asset to
      account_id_type  to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset            amount;

      /// User provided data encrypted to the memo key of the "to" account
      optional<memo_data> memo;
      extensions_type   extensions;

      account_id_type fee_payer()const { return from; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& k)const;
      share_type      calculate_fee(const fee_parameters_type& k, const asset_object& a)const;
   };

   /**
    * @ingroup operations
    *
    * @brief Transfers an amount of one asset from one account to another, support simple percentage based fee mode
    *
    *  Fees are paid by the "from" account
    *
    *  @pre amount.amount > 0
    *  @pre fee.amount >= 0
    *  @pre from != to
    *  @post from account's balance will be reduced by fee and amount
    *  @post to account's balance will be increased by amount
    *  @return n/a
    */
   struct transfer_v2_operation : public base_operation
   {
      struct fee_parameters_type {
         uint64_t fee             =  20 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte =  10 * GRAPHENE_BLOCKCHAIN_PRECISION; /// only required for large memos.
         /// Minimum fee amount, take effect when transfer asset with percentage based fee mode
         uint32_t min_fee         =   6 * GRAPHENE_BLOCKCHAIN_PRECISION;
         /// Maximum fee amount, take effect when transfer asset with percentage based fee mode
         uint32_t max_fee         = 300 * GRAPHENE_BLOCKCHAIN_PRECISION;
         /// the percentage, take effect when transfer asset with percentage based fee mode
         uint16_t percentage      = GRAPHENE_DEFAULT_TRANSFER_FEE_PERCENT;
      };

      asset            fee;
      /// Account to transfer asset from
      account_id_type  from;
      /// Account to transfer asset to
      account_id_type  to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset            amount;

      /// User provided data encrypted to the memo key of the "to" account
      optional<memo_data> memo;
      extensions_type   extensions;

      account_id_type fee_payer()const { return from; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& k)const; /// don't use it
      share_type      calculate_fee(const fee_parameters_type& k, const asset_object& a)const;
   };

   /**
    *  @class override_transfer_operation
    *  @brief Allows the issuer of an asset to transfer an asset from any account to any account if they have override_authority
    *  @ingroup operations
    *
    *  @pre amount.asset_id->issuer == issuer
    *  @pre issuer != from  because this is pointless, use a normal transfer operation
    */
   struct override_transfer_operation : public base_operation
   {
      struct fee_parameters_type {
         uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte = 10; /// only required for large memos.
      };

      asset           fee;
      account_id_type issuer;
      /// Account to transfer asset from
      account_id_type from;
      /// Account to transfer asset to
      account_id_type to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset amount;

      /// User provided data encrypted to the memo key of the "to" account
      optional<memo_data> memo;
      extensions_type   extensions;

      account_id_type fee_payer()const { return issuer; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& k)const;
   };

}} // graphene::chain

FC_REFLECT( graphene::chain::transfer_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::transfer_v2_operation::fee_parameters_type, (fee)(price_per_kbyte)(min_fee)(max_fee)(percentage) )
FC_REFLECT( graphene::chain::override_transfer_operation::fee_parameters_type, (fee)(price_per_kbyte) )

FC_REFLECT( graphene::chain::override_transfer_operation, (fee)(issuer)(from)(to)(amount)(memo)(extensions) )
FC_REFLECT( graphene::chain::transfer_operation, (fee)(from)(to)(amount)(memo)(extensions) )
FC_REFLECT( graphene::chain::transfer_v2_operation, (fee)(from)(to)(amount)(memo)(extensions) )
