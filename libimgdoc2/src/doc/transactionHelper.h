// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <utility>
#include <memory>

/// A utility in order to wrap a piece of code into a database-transaction.
///
/// \tparam t_return_value  Type of the return value.
template <typename t_return_value>
class TransactionHelper
{
private:
    std::function< t_return_value()> action_;
    std::shared_ptr<IDbConnection> database_connection_;
public:
    TransactionHelper(
        std::shared_ptr<IDbConnection> database_connection,
        std::function<t_return_value()> action) :
        action_(std::move(action)),
        database_connection_(std::move(database_connection))
    {}

    /// Execute the action guarded with Begin-/End-Transaction. Or, in more detail, what this is about
    /// is: 
    /// First thing to be aware of is that the DbConnection-objection is taking care of maintaing a "transaction state",
    /// i. e. whether currently we are inside a transaction. Background here is that nested transactions are not supported
    /// with SQLite (https://www.sqlite.org/lang_transaction.html), so a transaction is a "global state".
    /// So, what we do here, is
    /// - we query the DbConnection-object if there is a transaction pending  
    /// - if this is the case, we execute the action right away  
    /// - if not, we initiate a transaction, then call the action, then end the transaction  
    /// In other words - if there is no pending transaction, we wrap the action into a Begin-/End-Transaction.
    /// If the action is throwing an execption, we end the transaction with a rollback (i.e. again, only if we
    /// initiated the transaction).
    /// \returns {t_return_value} The return value of the action.
    t_return_value Execute()
    {
        bool transaction_initiated = false;
        if (!this->database_connection_->IsTransactionPending())
        {
            this->database_connection_->BeginTransaction();
            transaction_initiated = true;
        }

        try
        {
            t_return_value return_value = this->action_();

            if (transaction_initiated)
            {
                // TODO(JBL): I guess we need to think about how to deal with "exception from the next line"
                this->database_connection_->EndTransaction(true);
            }

            return return_value;
        }
        catch (...)
        {
            if (transaction_initiated)
            {
                this->database_connection_->EndTransaction(false);
            }

            throw;
        }
    }
};
