// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// Interface for controlling database transactions. A transaction is started with
    /// calling into BeginTransaction. All write/modify-operations after this call are then part
    /// of the transaction. The transaction is then completed by calling CommitTransaction, or
    /// dismissed by calling into RollbackTransaction. Note that nested transactions are not
    /// supported.
    /// </summary>
    public interface ITransaction
    {
        /// <summary> 
        /// Begins a transaction. Nested transactions are not allowed, calling BeginTransaction multiple times
        /// (i.e. without ending the transaction) results in an exception.
        /// </summary>
        void BeginTransaction();

        /// <summary> Commits a pending transaction.</summary>
        void CommitTransaction();

        /// <summary> Rolls back a pending transaction.</summary>
        void RollbackTransaction();
    }
}
