// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstring>
#include <cstdint>
#include <memory>
#include <exception>
#include "../IBlobOutput.h"

namespace imgdoc2
{
    /// \brief implementation of the IBlobOutput interface
    ///        
    /// An implementation of the IBlobOutput interface which allocates the data being delivered
    /// on the heap. The memory allocated is owned by the instance.
    class BlobOutputOnHeap : public imgdoc2::IBlobOutput
    {
    private:
        std::uint8_t* buffer_{ nullptr };
        size_t buffer_size_{ 0 };
        bool is_reserved_{ false };
    public:
        /// Destructor which releases the allocated memory.
        ~BlobOutputOnHeap() override
        {
            free(this->buffer_);
        }

        //! @copydoc imgdoc2::IBlobOutput::Reserve(size_t)
        [[nodiscard]] bool Reserve(size_t s) override
        {
            if (this->is_reserved_)
            {
                throw std::logic_error("This instance has already been initialized.");
            }

            this->buffer_ = static_cast<uint8_t*>(malloc(s));
            this->buffer_size_ = s;
            this->is_reserved_ = true;
            return true;
        }

        //! @copydoc imgdoc2::IBlobOutput::SetData(size_t, size_t, const void*)
        [[nodiscard]] bool SetData(size_t offset, size_t size, const void* data) override
        {
            if (!this->is_reserved_)
            {
                throw std::logic_error("'Reserve' was not called before.");
            }

            if (offset + size > this->buffer_size_)
            {
                throw std::invalid_argument("out-of-bounds");
            }

            memcpy(this->buffer_ + offset, data, size);
            return true;
        }
    public:
        /// Gets a boolean indicating whether output data has been reserved, in other words -
        /// whether "Reserve" was called.
        /// \returns True if output memory was reserved; false otherwise.
        [[nodiscard]] bool GetHasData() const { return this->is_reserved_; }

        /// Gets a const pointer to the data. The size of this buffer is given by "GetSizeOfData". If
        /// this instance has not been initialized, nullptr is returned.
        /// \returns If the instance is initialized, then a pointer to the buffer is returned; nullptr otherwise.
        [[nodiscard]] const std::uint8_t* GetDataC() const { return this->buffer_; }

        /// Gets a pointer to the data. The size of this buffer is given by "GetSizeOfData". If
        /// this instance has not been initialized, nullptr is returned.
        /// \returns If the instance is initialized, then a pointer to the buffer is returned; nullptr otherwise.
        [[nodiscard]] std::uint8_t* GetData() { return this->buffer_; }

        /// Gets size of data in bytes. It is usually advised to check "GetHasData()" before.
        /// \returns The size of the data in bytes.
        [[nodiscard]] size_t GetSizeOfData() const { return this->buffer_size_; }
    public:
        // no copy and no move 
        BlobOutputOnHeap() = default;
        BlobOutputOnHeap(const BlobOutputOnHeap&) = delete;             // copy constructor
        BlobOutputOnHeap& operator=(const BlobOutputOnHeap&) = delete;  // copy assignment
        BlobOutputOnHeap(BlobOutputOnHeap&&) = delete;                  // move constructor
        BlobOutputOnHeap& operator=(BlobOutputOnHeap&&) = delete;       // move assignment
    };
}
