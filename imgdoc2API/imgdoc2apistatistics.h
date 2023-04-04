
#pragma once

#include <atomic>
#include "imgdoc2statisticsinterop.h"

/// This struct is used to count active instances of objects, which are created by the imgdoc2API.
struct ImgDoc2ApiStatistics
{
    std::atomic_uint32_t number_of_createoptions_objects_active{ 0 };
    std::atomic_uint32_t number_of_openexistingoptions_objects_active{ 0 };
    std::atomic_uint32_t number_of_document_objects_active{ 0 };
    std::atomic_uint32_t number_of_reader2d_objects_active{ 0 };
    std::atomic_uint32_t number_of_writer2d_objects_active{ 0 };
    std::atomic_uint32_t number_of_reader3d_objects_active{ 0 };
    std::atomic_uint32_t number_of_writer3d_objects_active{ 0 };

    ImgDoc2StatisticsInterop GetInteropStruct() const
    {
        ImgDoc2StatisticsInterop interop;
        interop.number_of_createoptions_objects_active = this->number_of_createoptions_objects_active.load();
        interop.number_of_openexistingoptions_objects_active = this->number_of_openexistingoptions_objects_active.load();
        interop.number_of_document_objects_active = this->number_of_document_objects_active.load();
        interop.number_of_reader2d_objects_active = this->number_of_reader2d_objects_active.load();
        interop.number_of_writer2d_objects_active = this->number_of_writer2d_objects_active.load();
        interop.number_of_reader3d_objects_active = this->number_of_reader3d_objects_active.load();
        interop.number_of_writer3d_objects_active = this->number_of_writer3d_objects_active.load();
        return interop;
    }
};
