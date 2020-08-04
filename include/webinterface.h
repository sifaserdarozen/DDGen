/**
 * @file
 * @brief web interface
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#include "libhttp.h"

#include <string>

namespace ddgen {

class WebInterface
{
public:
    explicit WebInterface(bool shouldUseSecureWebInterface);
    ~WebInterface();

private:
    static int begin_request_handler(struct lh_ctx_t* ctx, struct lh_con_t* conn);

    lh_ctx_t* ctx;
};

} // namespace ddgen
