/**
 * @file
 * @brief Basic socket information
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

namespace ddgen
{

/**
 * @brief ip port combination
 *
 * Ip and port combination
 */
struct IpPort
{
    unsigned int m_ipv4;    /**< ip v4 information */
    unsigned short int m_port;    /**< port information */

    /**
    * @brief Constructor
    *
    * constructor that sets ipv4 and port pair
    * @param ipv4 INPUT ipv4 address
    * @param port INPUT port information
    */
    IpPort(unsigned int ipv4 = 0, unsigned short int port = 0) : m_ipv4(ipv4), m_port(port) {}
};

}