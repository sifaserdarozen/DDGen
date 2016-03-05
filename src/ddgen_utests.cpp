/** @file ddgen_utests.cpp
 *
 * Unit tests are performed with Catch v1.3.5
 * More information about catch may be seen at their site https://github.com/philsquared/Catch
 */

// standard libraries
#include "iostream"
#include "cstring"    // for std::memcmp

// 3rd party libraries
#define CATCH_CONFIG_MAIN    // provides creation of executable, should be above catch.hpp
#include "catch.hpp"

#include "test.h"
#include "rawsocket.h"

TEST_CASE( "Rtp Header Tests", "[RtpHeaderType]" )
{
    SECTION( "reading rtp header from array " )
    {
        RtpHeaderType rtp_header;
        rtp_header.ReadFromBuffer(sample_line_rtp_header);

        REQUIRE( sample_rtp_header == rtp_header );
    }
    
    SECTION( "writing rtp header to array " )
    {
        unsigned char line_rtp[rtp_header_size];
        sample_rtp_header.WriteToBuffer(line_rtp);

        REQUIRE( 0 == std::memcmp(sample_line_rtp_header, line_rtp, rtp_header_size));
    }
}

TEST_CASE( "Ipv4 Header Tests", "[Ipv4HeaderType]" )
{
    SECTION( "reading ipv4 header from array" )
    {
        Ipv4HeaderType ipv4_header;
        ipv4_header.ReadFromBuffer(sample_line_ipv4_header);

        REQUIRE( sample_ipv4_header == ipv4_header );
    }
    
    SECTION( "calculating checksum and writing ipv4 header to array " )
    {
        Ipv4HeaderType ipv4_header;
        ipv4_header.ReadFromBuffer(sample_line_ipv4_header);
        ipv4_header.checksum = 0;
        
        unsigned char line_ipv4[ipv4_header_size];
        
        // write without updating checksum should produce a false in checksum check
        ipv4_header.WriteToBuffer(line_ipv4);
        REQUIRE( false == CheckIpv4Checksum(line_ipv4) );
        
        // write with updating checksum should both pass checksum test and be equal to reference array
        ipv4_header.UpdateChecksumWriteToBuffer(line_ipv4);
        REQUIRE( true == CheckIpv4Checksum(line_ipv4) );
        REQUIRE( 0 == std::memcmp(sample_line_ipv4_header, line_ipv4, ipv4_header_size));
    }
}

TEST_CASE( "Udp Header Tests", "[UdpHeaderType]" )
{
    SECTION( "reading udp header from array" )
    {
        UdpHeaderType udp_header;
        udp_header.ReadFromBuffer(sample_line_udp_header);

        REQUIRE( sample_udp_header == udp_header );
    }
    
    SECTION( "calculating checksum and writing udp header to array " )
    {
        UdpHeaderType udp_header;
        udp_header.ReadFromBuffer(sample_line_udp_header);

        udp_header.checksum = 0;
        
        PseudoIpv4HeaderType pseudo_ipv4_header;
        pseudo_ipv4_header.src_addr = sample_ipv4_header.src_addr;
        pseudo_ipv4_header.dst_addr = sample_ipv4_header.dst_addr;
        pseudo_ipv4_header.protocol = sample_ipv4_header.protocol;
        pseudo_ipv4_header.data_len = udp_header.tot_len;
        
        unsigned char line_udp[udp_header_size + sizeof(sample_line_rtp_packet)];
        // copy udp data with sample
        std::memcpy((line_udp + udp_header_size), sample_line_rtp_packet, sizeof(sample_line_rtp_packet));
        
        // write without updating checksum should produce a false in checksum check
        udp_header.WriteToBuffer(line_udp);
        REQUIRE( false == CheckUdpChecksum(line_udp, pseudo_ipv4_header) );
        
        // write with updating checksum should both pass checksum test and be equal to reference array
        udp_header.UpdateChecksumWriteToBuffer(line_udp, sample_line_rtp_packet, pseudo_ipv4_header);
        REQUIRE( true == CheckUdpChecksum(line_udp, pseudo_ipv4_header) );
        REQUIRE( 0 == std::memcmp(sample_line_udp_header, line_udp, udp_header_size));
    }
}
