# PackedFilter
A pattern matching algorithm. In essence it does a fast naive search by checking 8 characters at a time at the bit level (a boolean array is a good visual). This has the added benefit of being able to check multiple 
potential matches in parallel and is compatible with any datatype that is represented in binary. No indexing or preprocessing needed.
