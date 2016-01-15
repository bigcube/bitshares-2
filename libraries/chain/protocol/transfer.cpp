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
#include <graphene/chain/protocol/transfer.hpp>

namespace graphene { namespace chain {

share_type transfer_operation::calculate_fee( const fee_parameters_type& schedule )const
   // FIXME need hard fork check logic here or somewhere else for backward compatibility.
   FC_THROW( "Deprecated. Use calculate_fee( const fee_parameters_type& schedule, const asset_object& asset) instead." );
}

share_type transfer_operation::calculate_fee( const fee_parameters_type& schedule, const asset_object& asset)const
{
   share_type core_fee_required;
   optional<asset_options::ext::transfer_fee_options> o = asset.get_transfer_fee_options();
   if( !o.valid() || o.fee_mode == asset_transfer_fee_mode_flat ) // flat fee mode
   {
      core_fee_required = schedule.fee;
   }
   else if( o.fee_mode == asset_transfer_fee_mode_percentage_simple ) // simple percentage fee mode
   {
      // need to know CER of amount.asset_id so that fee can be calculated
      // fee = amount.amount * ~asset.CER * asset.transfer_fee_options.percentage
      auto percent_amout = fc::uint128(amount.amount);
      percent_amount *= o.percentage;
      percent_amount /= GRAPHENE_100_PERCENT;
      auto core_fee = asset( percent_amount.to_int64(), amount.asset_id ) * ( ~asset.core_exchange_rate );
      core_fee_required = core_fee.amount;
      if( core_fee_required < schedule.min_fee ) core_fee_required = schedule.min_fee;
      if( core_fee_required > schedule.max_fee ) core_fee_required = schedule.max_fee;
   }
   if( memo )
      core_fee_required += calculate_data_fee( fc::raw::pack_size(memo), schedule.price_per_kbyte );
   return core_fee_required;
}


void transfer_operation::validate()const
{
   FC_ASSERT( fee.amount >= 0 );
   FC_ASSERT( from != to );
   FC_ASSERT( amount.amount > 0 );
}



share_type override_transfer_operation::calculate_fee( const fee_parameters_type& schedule )const
{
   share_type core_fee_required = schedule.fee;
   if( memo )
      core_fee_required += calculate_data_fee( fc::raw::pack_size(memo), schedule.price_per_kbyte );
   return core_fee_required;
}


void override_transfer_operation::validate()const
{
   FC_ASSERT( fee.amount >= 0 );
   FC_ASSERT( from != to );
   FC_ASSERT( amount.amount > 0 );
   FC_ASSERT( issuer != from );
}

} } // graphene::chain
