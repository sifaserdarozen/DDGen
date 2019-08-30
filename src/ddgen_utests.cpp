/** @file ddgen_utests.cpp
 *
 * Unit tests are performed with Catch v1.3.5
 * More information about catch may be seen at their site https://github.com/philsquared/Catch
 */

#include "jsontype.h"
#include "rawsocket.h"
#include "test.h"

#define CATCH_CONFIG_MAIN    // provides creation of executable, should be above catch.hpp
#include "catch.hpp"

#include <iostream>
#include <cstring>

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

TEST_CASE( "Json test", "[JsonType]" )
{
    SECTION( "reading from string" )
    {
        JsonType message_in_json_test1("{\"title\": \"Eternal Sunshine of the Spotless Mind\"}");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\"}" == message_in_json_test1.ToString() );

        JsonType message_in_json_test2(" { \"title\"  :   \"Eternal Sunshine of the Spotless Mind\" } ");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\"}" == message_in_json_test2.ToString() );

        JsonType message_in_json_test3("{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\"}");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\"}" == message_in_json_test3.ToString() );

        JsonType message_in_json_test4("{\"director\": {\"name\": \"Michel Gondry\"}, \"year\": \"2004\"}");
        REQUIRE( "{\"director\": {\"name\": \"Michel Gondry\"}, \"year\": \"2004\"}" == message_in_json_test4.ToString() );

        JsonType message_in_json_test5("{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\", \"cast\" : [{\"name\": \"Jim Carrey\"}, {\"name\": \"Kate Winslet\"}]}");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\", \"cast\": [{\"name\": \"Jim Carrey\"}, {\"name\": \"Kate Winslet\"}]}" == message_in_json_test5.ToString() );

        REQUIRE( "Eternal Sunshine of the Spotless Mind" == message_in_json_test5["title"] );

        message_in_json_test5.Add("genre", "thriller");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\", \"cast\": [{\"name\": \"Jim Carrey\"}, {\"name\": \"Kate Winslet\"}], \"genre\": \"thriller\"}" == message_in_json_test5.ToString() );

        message_in_json_test5.Add("genre", "drama");
        REQUIRE( "{\"title\": \"Eternal Sunshine of the Spotless Mind\", \"year\": \"2004\", \"cast\": [{\"name\": \"Jim Carrey\"}, {\"name\": \"Kate Winslet\"}], \"genre\": \"drama\"}" == message_in_json_test5.ToString() );

        JsonType message_in_json_test6;
        message_in_json_test6.Add("title", "Eternal Sunshine Of The Spotless Mind");
        message_in_json_test6.Add("{\"cast\": [{\"name\": \"Jim\"}, {\"name\": \"Kate\"}]}");
        REQUIRE( "{\"title\": \"Eternal Sunshine Of The Spotless Mind\", \"cast\": [{\"name\": \"Jim\"}, {\"name\": \"Kate\"}]}" == message_in_json_test6.ToString() );

        std::string adaptor = "{\"adaptors\": [{\"name\": \"\\Device\\NPF_{A3A00374-6E04-4A3D-8383-752A280E3B81}\", \"description\": \"Microsoft\", \"ip\": \"0.0.0.0\", \"stat\": \"0\"}, {\"name\": \"\\Device\\NPF_{3B50F57D-2855-4B0D-A67C-5223C29458C3}\", \"description\": \"Microsoft\", \"ip\": \"192.168.1.7\", \"stat\": \"0\"}]}";
        JsonType message_in_json_test7;
        message_in_json_test7.Add("message", "ListAdaptorsAck");
        message_in_json_test7.Add(adaptor);
        REQUIRE( "{\"message\": \"ListAdaptorsAck\", \"adaptors\": [{\"name\": \"\\\\Device\\\\NPF_{A3A00374-6E04-4A3D-8383-752A280E3B81}\", \"description\": \"Microsoft\", \"ip\": \"0.0.0.0\", \"stat\": \"0\"}, {\"name\": \"\\\\Device\\\\NPF_{3B50F57D-2855-4B0D-A67C-5223C29458C3}\", \"description\": \"Microsoft\", \"ip\": \"192.168.1.7\", \"stat\": \"0\"}]}" == message_in_json_test7.ToString() );

        JsonType message_in_json_test8("{\"message\": \"IpList\", \"ips\": [\"192.168.2.3\", \"192.168.2.4\", \"192.168.2.5\"]}");
        REQUIRE("{\"message\": \"IpList\", \"ips\": [\"192.168.2.3\", \"192.168.2.4\", \"192.168.2.5\"]}" == message_in_json_test8.ToString() );
        REQUIRE("[\"192.168.2.3\", \"192.168.2.4\", \"192.168.2.5\"]" == message_in_json_test8["ips"] );
        std::vector<std::string> ips = message_in_json_test8.GetStrArrValue("ips");
        REQUIRE( 3 == ips.size());
        REQUIRE( "192.168.2.4" == ips[1] );

        JsonType message_in_json_test9;
        message_in_json_test9.Add("path", "D:\\What\\the\\blepp");
        REQUIRE( "{\"path\": \"D:\\\\What\\\\the\\\\blepp\"}" == message_in_json_test9.ToString());
        JsonType message_in_json_test10("{\"policy\": {\"user\": \"wr\"}}");
        REQUIRE( "{\"policy\": {\"user\": \"wr\"}}" == message_in_json_test10.ToString());
        message_in_json_test10.Add("{\"new_policy\": {\"group\": \"wrx\"}}");
        REQUIRE( "{\"policy\": {\"user\": \"wr\"}, \"new_policy\": {\"group\": \"wrx\"}}" == message_in_json_test10.ToString());
        message_in_json_test10.Add("\"past_policy\": {\"user\": \"r\"}");
        REQUIRE( "{\"policy\": {\"user\": \"wr\"}, \"new_policy\": {\"group\": \"wrx\"}, \"past_policy\": {\"user\": \"r\"}}" == message_in_json_test10.ToString());
        message_in_json_test9.Add(message_in_json_test10.ToString());
        REQUIRE( "{\"path\": \"D:\\\\What\\\\the\\\\blepp\", \"policy\": {\"user\": \"wr\"}, \"new_policy\": {\"group\": \"wrx\"}, \"past_policy\": {\"user\": \"r\"}}" == message_in_json_test9.ToString());
    }
}
