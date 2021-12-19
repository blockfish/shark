// 2021 iitalics
#pragma once

#include "blockfish/tbp/msg.h"

namespace blockfish {
namespace tbp {

/**
 * Abstraction over sending/recieving message.
 */
class io {
 public:
    io() = default;

    virtual ~io();

    /**
     * Send the given message next as soon as possible.
     */
    virtual void send(const tx_msg& msg) = 0;

    /**
     * If a new message is immediately avaiable, sets 'msg' to the contents of the
     * incoming message. Otherwise 'msg' is left unchanged.
     */
    virtual void recv(rx_msg* msg) = 0;
};

}   // namespace tbp
}   // namespace blockfish

