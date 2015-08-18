--- Creates an empty array with preallocated storage.
-- @param narr The number of preallocated array elements.
-- @param nrec The number of preallocated non-array elements.
function table.createtable(narr, nrec)
    -- Implemented via a C callback.
    return createtable_c(narr, nrec)
end