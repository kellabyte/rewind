#include "iostream"
#include "lmdb.h"

const int SUCCESS = 0;

class SuccessFrom {
    const int rc;
public:
    SuccessFrom( int rc ) : rc(rc) {}

    explicit operator bool() const {
        return rc == SUCCESS;
    }
    friend std::ostream& operator<<( std::ostream& os, SuccessFrom const& result ) {
        return os << mdb_strerror(result.rc);
    }
};