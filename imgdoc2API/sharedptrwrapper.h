// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <imgdoc2.h>

constexpr uint32_t kMagicInvalid = 0;
constexpr uint32_t kMagicIHostingEnvironment = 0xBCFB6C34;
constexpr uint32_t kMagicIDoc = 0x5F3D69B2;
constexpr uint32_t kMagicIDocRead2d = 0xA31445DC;
constexpr uint32_t kMagicIDocRead3d = 0x2762E513;
constexpr uint32_t kMagicIDocWrite2d = 0xABFF9A83;
constexpr uint32_t kMagicIDocWrite3d = 0x1714CBB3;
constexpr uint32_t kMagicIOpenExistingOptions = 0xE8AD8F14;
constexpr uint32_t kMagicICreateOptions = 0x229D2DAA;

// In this file we define a generic template class that can be used to wrap a shared pointer to an object.
// This is used to provide a handle to an object, and we use a magic value to check if the handle is still valid.

/// This class is used to represent a shared pointer, or this is what the handles we are providing is pointing to. This class
/// contains a magic value, which is used to check if the handle is still valid. When the handle is created, the magic value
/// is set to a specific value. When the handle is destroyed, the magic value is set to 0. If the magic value is not the
/// expected value, the handle is invalid.
/// We use template specialization to set the magic value for each type of object, and we need to use some template magic
/// to make this work. We use partial template specialization to set the magic value for each type of object. 
///
/// \typeparam  ClassT      The type of the object to be stored here.
/// \typeparam  MagicValueN The magic value to be used for this type of object.
template <typename ClassT, std::uint32_t MagicValueN>
struct SharedPtrWrapperBase
{
    explicit SharedPtrWrapperBase(std::shared_ptr<ClassT> shared_ptr) : magic_(MagicValueN), shared_ptr_(std::move(shared_ptr)) {}

    ~SharedPtrWrapperBase()
    {
        this->Invalidate();
    }

    /// Query if this object is valid. This checks if the magic value is the expected value. If this is not the case,
    /// this means that either the value has been invalided, or that the pointer is bogus.
    ///
    /// \returns    True if valid, false if not.
    [[nodiscard]] bool IsValid() const { return this->magic_ == MagicValueN; }

    /// Invalidates the magic value. This is used when the handle is destroyed.
    void Invalidate() { this->magic_ = kMagicInvalid; }

    std::uint32_t magic_;
    std::shared_ptr<ClassT> shared_ptr_;
};

/// This class is used to represent a pointer (to an object), or this is what the handles we are providing is pointing to. This class
/// contains a magic value, which is used to check if the handle is still valid. When the handle is created, the magic value
/// is set to a specific value. When the handle is destroyed, the magic value is set to 0. If the magic value is not the
/// expected value, the handle is invalid.
/// We use template specialization to set the magic value for each type of object, and we need to use some template magic
/// to make this work. We use partial template specialization to set the magic value for each type of object. 
/// Note that the pointer is deleted when the handle is destroyed, so there is a transfer of ownership of the pointer/object.
///
/// \typeparam  ClassT      The type of the object to be stored here.
/// \typeparam  MagicValueN The magic value to be used for this type of object.
template <typename ClassT, std::uint32_t MagicValueN>
struct PtrWrapperBase
{
    explicit PtrWrapperBase(ClassT* ptr) : magic_(MagicValueN), ptr_(ptr) {}

    ~PtrWrapperBase()
    {
        this->Invalidate();
        delete this->ptr_;
        this->ptr_ = nullptr;
    }

    /// Query if this object is valid. This checks if the magic value is the expected value. If this is not the case,
    /// this means that either the value has been invalided, or that the pointer is bogus.
    ///
    /// \returns    True if valid, false if not.
    [[nodiscard]] bool IsValid() const { return this->magic_ == MagicValueN; }

    /// Invalidates the magic value. This is used when the handle is destroyed.
    void Invalidate() { this->magic_ = kMagicInvalid; }

    std::uint32_t magic_;
    ClassT* ptr_;
};

/// SharedPtrWrapper is a generic template class that inherits from SharedPtrWrapperBase.
/// This uses an invalid magic value of 0 for all types, and we use partial template specialization to set the magic value
/// for each type of object we want to support.
/// Note that the constructor is private, so that we can only create partial specialization of this class.
///
/// \typeparam  t   Generic type parameter.
template <typename t>
struct SharedPtrWrapper :public SharedPtrWrapperBase<t, 0>
{
private:
    explicit SharedPtrWrapper(std::shared_ptr<t> shared_ptr) : SharedPtrWrapperBase<t, 0>(std::move(shared_ptr)) {}
};

/// PtrWrapper is a generic template class that inherits from PtrWrapperBase.
/// This uses an invalid magic value of 0 for all types, and we use partial template specialization to set the magic value
/// for each type of object we want to support.
/// Note that the constructor is private, so that we can only create partial specialization of this class.
///
/// \typeparam  t   Generic type parameter.
template <typename t>
struct PtrWrapper :public PtrWrapperBase<t, 0>
{
private:
    explicit PtrWrapper(t* ptr) : PtrWrapperBase<t, 0>(ptr) {}
};

/// Partial template specialization for IHostingEnvironment objects.
template <>
struct SharedPtrWrapper<imgdoc2::IHostingEnvironment> : SharedPtrWrapperBase<imgdoc2::IHostingEnvironment, kMagicIHostingEnvironment>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IHostingEnvironment> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IHostingEnvironment, kMagicIHostingEnvironment>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IDoc objects.
template <>
struct SharedPtrWrapper<imgdoc2::IDoc> : SharedPtrWrapperBase<imgdoc2::IDoc, kMagicIDoc>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IDoc> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IDoc, kMagicIDoc>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IDocRead2d objects.
template <>
struct SharedPtrWrapper<imgdoc2::IDocRead2d> : SharedPtrWrapperBase<imgdoc2::IDocRead2d, kMagicIDocRead2d>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IDocRead2d> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IDocRead2d, kMagicIDocRead2d>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IDocRead3d objects.
template <>
struct SharedPtrWrapper<imgdoc2::IDocRead3d> : SharedPtrWrapperBase<imgdoc2::IDocRead3d, kMagicIDocRead3d>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IDocRead3d> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IDocRead3d, kMagicIDocRead3d>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IDocWrite2d objects.
template <>
struct SharedPtrWrapper<imgdoc2::IDocWrite2d> : SharedPtrWrapperBase<imgdoc2::IDocWrite2d, kMagicIDocWrite2d>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IDocWrite2d> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IDocWrite2d, kMagicIDocWrite2d>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IDocWrite3d objects.
template <>
struct SharedPtrWrapper<imgdoc2::IDocWrite3d> : SharedPtrWrapperBase<imgdoc2::IDocWrite3d, kMagicIDocWrite3d>
{
    explicit SharedPtrWrapper(std::shared_ptr<imgdoc2::IDocWrite3d> shared_ptr) :
        SharedPtrWrapperBase<imgdoc2::IDocWrite3d, kMagicIDocWrite3d>(std::move(shared_ptr)) {}
};

/// Partial template specialization for IOpenExistingOptions objects - this is using plain-pointers.
template <>
struct PtrWrapper<imgdoc2::IOpenExistingOptions> : PtrWrapperBase<imgdoc2::IOpenExistingOptions, kMagicIOpenExistingOptions>
{
    explicit PtrWrapper(imgdoc2::IOpenExistingOptions* ptr) :
        PtrWrapperBase<imgdoc2::IOpenExistingOptions, kMagicIOpenExistingOptions>(ptr) {}
};

/// Partial template specialization for ICreateOptions objects - this is using plain-pointers.
template <>
struct PtrWrapper<imgdoc2::ICreateOptions> : PtrWrapperBase<imgdoc2::ICreateOptions, kMagicICreateOptions>
{
    explicit PtrWrapper(imgdoc2::ICreateOptions* ptr) :
        PtrWrapperBase<imgdoc2::ICreateOptions, kMagicICreateOptions>(ptr) {}
};
