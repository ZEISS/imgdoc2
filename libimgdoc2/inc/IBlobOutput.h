// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// \brief interface for outputting binary data
    ///        
    /// This interface is used to output binary data from libimgdoc2. It is passed in to 
    /// the corresponding operation, then the "Reserve"-method is called for advertising
    /// the size of the data. Then (multiple) calls to "SetData" follow, where the object
    /// is requested to copy data into its internal storage.
    class IBlobOutput
    {
    public:
        /// Notifies about the size of data to be returned. 
        /// This method must only be called once, additional calls should result in an
        /// exception. If this method returns false, it is interpreted as "the client is
        /// not interested in the data" and there will not be subsequent calls to "SetData".
        /// \param  s The size of the data being returned.
        /// \returns True if it succeeds (and the object is ready to handle "SetData" calls), false if it fails.
        virtual bool Reserve(size_t s) = 0;

        /// Data is passed to the "blob-output"-object with this call. The pointer "data" is
        /// only assumed to be valid for the duration of this call - in other words, the object
        /// needs to access the memory only within the call (and e.g. make a copy of the data).
        /// The address (indicated by offset and size) must be completely within the size as 
        /// advertised with the "Reserve"-call. The method should validate this condition and
        /// throw an exception in case of a violation.
        /// If the method returns "false", it is interpreted "the object is not interested in any
        /// additional data", and subsequent calls may be surpressed.
        /// \param  offset    The offset.
        /// \param  size      The size.
        /// \param  data The data.
        /// \returns True if it succeeds, false if it fails.
        virtual bool SetData(size_t offset, size_t size, const void* data) = 0;

        virtual ~IBlobOutput() = default;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IBlobOutput() = default;
        IBlobOutput(const IBlobOutput&) = delete;             // copy constructor
        IBlobOutput& operator=(const IBlobOutput&) = delete;  // copy assignment
        IBlobOutput(IBlobOutput&&) = delete;                  // move constructor
        IBlobOutput& operator=(IBlobOutput&&) = delete;       // move assignment
    };
}
