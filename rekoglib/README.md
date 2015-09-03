# RekogLib 

Literally "Recognition Library" but I decided to change it to "Rekog" because Kog'Maw. There are two things that we want to train to recognize: 

* Images (Champions Portraits (or lack thereof), Event Icons, Items)
* Text (Time, Score, etc.)

These are two separate paths that can use vastly different algorithms/backends so therefore they will be kept separate and each section Offline/Online/Shared has a separate path for each section. In the future, it will be necessary to make sure that the library is generalized enough for different games.

## Offline (Programs)

Provides tools and utilities to train our dataset(s). Will use the 'Shared' library as well as the 'Online' library.

## Online (Shared Library)

Provides an interface by which an external program can load and interact with the trained datasets. Will use the 'Shared' library.

## Shared (Static Library)

Does the bulk of the heavy lifting to perform training and recognition on the input images/text.
