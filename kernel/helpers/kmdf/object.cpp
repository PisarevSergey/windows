#include "object.h"

WDF_OBJECT_ATTRIBUTES kmdf::CreateObjectAttributes(WDFOBJECT parent, PFN_WDF_OBJECT_CONTEXT_CLEANUP cleanup) {
    WDF_OBJECT_ATTRIBUTES objectAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
    objectAttributes.ParentObject = parent;
    objectAttributes.EvtCleanupCallback = cleanup;

    return objectAttributes;
}
