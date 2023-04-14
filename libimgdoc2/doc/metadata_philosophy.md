# Approach and Philosophy: Multi-Level Organization of Microscopy Image Data           {#approach_and_philosophy}

In the following sections, we will describe the approach and philosophy underlying the design
and implementation of imgdoc2.

## Multi-Level Organization of Microscopy Image Data: Emphasizing Context, Relationships, and Standardized Terminology


The following scheme is suggested as a general approach to organizing microscopy image data, where
fundamentally three levels of organization are distinguished:

| Level | Name                  | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
|-------|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1     | set of images         | This level represents a collection of microscopy images that do not have any coherent relationships. These images may come from different samples, time points, or experimental conditions, and they are not explicitly linked by spatial or temporal coordinates.                                                                                                                                                                                                                                     |
| 2     | ordered set of images | At this level, we introduce the concept of "dimensions" and "coordinates" to assign e.g. spatial or temporal relationships between images. This creates an ordered set of images, where each image is associated with specific coordinates in space or time. Ordered sets of images are particularly useful for users working with time-lapse, z-stack, or multi-channel data, as it allows for easy navigation and analysis of the image data in the context of its spatial or temporal organization. |
| 3     | microscopic image     | The third and most detailed level of organization is the "microscopic image," where the specific microscopy modality used to acquire the image is explicitly defined. At this level, users provide information about the type of microscopy employed (e.g., brightfield, fluorescence, confocal, electron microscopy), as well as any pertinent settings, techniques, or experimental conditions.                                                                                                      |

| 1                                                     | 2                                                                     | 3                                                             |
|-------------------------------------------------------|-----------------------------------------------------------------------|---------------------------------------------------------------|
| ![set of images](images/philosophy_set_of_images.png) | ![ordered set of images](images/philosophy_ordered_set_of_images.png) | ![microscopic image](images/philosophy_microscopic_image.png) |

To facilitate clear communication and understanding of the microscopic image data, a standardized terminology 
and scheme are necessary. This structured framework will encompass all relevant terms and concepts related 
to microscopy, including sample preparation, imaging modalities, acquisition parameters, and post-processing 
techniques. By adhering to a well-defined set of terms and structures, users can accurately describe their 
image data, ensuring effective data sharing, interpretation, and analysis within the scientific community.

By incorporating these three levels of organization into our document format, we aim to provide a versatile 
solution for managing microscopy image data that caters to the diverse needs of researchers and microscopy 
applications. With a clearly defined system for organizing and describing image data, users can effectively 
store, navigate, and analyze their images while maintaining the context and details necessary for 
accurate interpretation and reproducibility.