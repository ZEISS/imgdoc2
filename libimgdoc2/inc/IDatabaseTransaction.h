// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// \brief Interface for controlling database transactions.
    ///
    /// A transaction is started with calling into BeginTransaction. All write/modify-operations after this 
    /// call are then part of the transaction. The transaction is then completed by calling CommitTransaction,
    /// or dismissed by calling into RollbackTransaction.
    /// Note that nested transactions are not supported. 
    class IDatabaseTransaction
    {
    public:
        /// Begins a transaction. Nested transactions are not allowed, calling BeginTransaction multiple times
        /// (i.e. without ending the transaction) results in an exception.
        virtual void BeginTransaction() = 0;

        /// Commits a pending transaction.
        virtual void CommitTransaction() = 0;


        /// Rolls back a pending transaction.
        virtual void RollbackTransaction() = 0;
        virtual ~IDatabaseTransaction() = default;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDatabaseTransaction() = default;
        IDatabaseTransaction(const IDatabaseTransaction&) = delete;             // copy constructor
        IDatabaseTransaction& operator=(const IDatabaseTransaction&) = delete;  // copy assignment
        IDatabaseTransaction(IDatabaseTransaction&&) = delete;                  // move constructor
        IDatabaseTransaction& operator=(IDatabaseTransaction&&) = delete;       // move assignment
    };
}
