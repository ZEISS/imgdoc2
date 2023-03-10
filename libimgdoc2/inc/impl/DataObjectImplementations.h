// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstring>
#include "../IDataObj.h"

namespace imgdoc2
{
    /// An implementation of the IDataObjBase interface. This class allocates and owns memory on the heap.
    class DataObjectOnHeap : public imgdoc2::IDataObjBase
    {
    private:
        std::uint8_t* buffer_{ nullptr };
        size_t buffer_size_{ 0 };
    public:
        /// Constructor which allocates the specified amount of data on the heap.
        /// \param  size The size of the buffer to allocate (in bytes).
        explicit DataObjectOnHeap(size_t size)
        {
            this->buffer_ = static_cast<std::uint8_t*>(malloc(size));
            this->buffer_size_ = size;
        }

        //! @copydoc imgdoc2::IDataObjBase::GetData(const void**, size_t*) const
        void GetData(const void** p, size_t* s) const override
        {
            if (p != nullptr)
            {
                *p = this->buffer_;
            }

            if (s != nullptr)
            {
                *s = this->buffer_size_;
            }
        }

        ~DataObjectOnHeap() override
        {
            free(this->buffer_);
        }

    public:
        /// Gets a const pointer to the data. The size of this buffer is given by "GetSizeOfData".
        [[nodiscard]] const std::uint8_t* GetDataC() const { return this->buffer_; }

        /// Gets a pointer to the data. The size of this buffer is given by "GetSizeOfData".
        [[nodiscard]] std::uint8_t* GetData() { return this->buffer_; }

        /// Gets size of data in bytes.
        /// \returns The size of the data in bytes.
        [[nodiscard]] size_t GetSizeOfData() const { return this->buffer_size_; }
    public:
        // no copy and no move 
        DataObjectOnHeap() = delete;
        DataObjectOnHeap(const DataObjectOnHeap&) = delete;             // copy constructor
        DataObjectOnHeap& operator=(const DataObjectOnHeap&) = delete;  // copy assignment
        DataObjectOnHeap(DataObjectOnHeap&&) = delete;                  // move constructor
        DataObjectOnHeap& operator=(DataObjectOnHeap&&) = delete;       // move assignment
    };
}
