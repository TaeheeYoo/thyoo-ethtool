/****************************************************************************
 * Test cases for ethtool command-line parsing
 * Copyright 2011 Solarflare Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#include <stdio.h>
#include <stdlib.h>
#define TEST_NO_WRAPPERS
#include "internal.h"

#ifdef ETHTOOL_ENABLE_NETLINK
#define IS_NL 1
#else
#define IS_NL 0
#endif

static struct test_case {
	int rc;
	const char *args;
} test_cases[] = {
	{ 1, "" },
	{ 0, "devname" },
	{ 0, "15_char_devname" },
	{ !IS_NL, "--json devname" },
	/* netlink interface allows names up to 127 characters */
	{ !IS_NL, "16_char_devname!" },
	{ !IS_NL, "127_char_devname0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde" },
	{ 1, "128_char_devname0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef" },
	/* Argument parsing for -s is specialised */
	{ 0, "-s devname" },
	{ 0, "--change devname speed 100 duplex half mdix auto" },
	{ 1, "-s devname speed foo" },
	{ 1, "--change devname speed" },
	{ 0, "-s devname duplex half" },
	{ 1, "--change devname duplex foo" },
	{ 1, "-s devname duplex" },
	{ 1, "--change devname mdix foo" },
	{ 1, "-s devname mdix" },
	{ 0, "--change devname port tp" },
	{ 1, "-s devname port foo" },
	{ 1, "--change devname port" },
	{ 0, "-s devname autoneg on" },
	{ 1, "--change devname autoneg foo" },
	{ 1, "-s devname autoneg" },
	{ 0, "--change devname advertise 0x1" },
	{ 0, "--change devname advertise 0xf" },
	{ 0, "--change devname advertise 0Xf" },
	{ 0, "--change devname advertise 1" },
	{ 0, "--change devname advertise f" },
	{ 0, "--change devname advertise 01" },
	{ 0, "--change devname advertise 0f" },
	{ 0, "--change devname advertise 0xfffffffffffffffffffffffffffffffff" },
	{ 0, "--change devname advertise fffffffffffffffffffffffffffffffff" },
	{ 0, "--change devname advertise 0x0000fffffffffffffffffffffffffffff" },
	{ 0, "--change devname advertise 0000fffffffffffffffffffffffffffff" },
	{ 1, "-s devname advertise" },
	{ 1, "-s devname advertise 0x" },
	{ 1, "-s devname advertise foo" },
	{ 1, "-s devname advertise 0xfoo" },
	{ 1, "--change devname advertise" },
	{ 0, "-s devname phyad 1" },
	{ 1, "--change devname phyad foo" },
	{ 1, "-s devname phyad" },
	/* Deprecated 'xcvr' detected by netlink parser */
	{ IS_NL, "--change devname xcvr external" },
	{ 1, "-s devname xcvr foo" },
	{ 1, "--change devname xcvr" },
	{ 0, "-s devname wol p" },
	{ 1, "--change devname wol" },
	{ 0, "-s devname sopass 01:23:45:67:89:ab" },
	{ 1, "--change devname sopass 01:23:45:67:89:" },
	{ 1, "-s devname sopass 01:23:45:67:89" },
	{ 1, "--change devname sopass" },
	{ 0, "-s devname msglvl 1" },
	{ 1, "--change devname msglvl" },
	{ 0, "-s devname msglvl hw on rx_status off" },
	{ 1, "--change devname msglvl hw foo" },
	{ 1, "-s devname msglvl hw" },
	{ 0, "--change devname speed 100 duplex half port tp autoneg on advertise 0x1 phyad 1 wol p sopass 01:23:45:67:89:ab msglvl 1" },
	/* Deprecated 'xcvr' detected by netlink parser */
	{ IS_NL, "--change devname speed 100 duplex half port tp autoneg on advertise 0x1 phyad 1 xcvr external wol p sopass 01:23:45:67:89:ab msglvl 1" },
	{ 1, "-s devname foo" },
	{ 1, "-s" },
	{ 0, "-a devname" },
	{ 0, "--show-pause devname" },
	{ 1, "-a" },
	/* Many other sub-commands use parse_generic_cmdline() and
	 * don't need to be check in that much detail. */
	{ 0, "-A devname autoneg on" },
	{ 1, "--pause devname autoneg foo" },
	{ 1, "-A devname autoneg" },
	{ 0, "--pause devname rx off" },
	{ 0, "-A devname tx on rx on autoneg off" },
	{ 1, "--pause devname foo on" },
	{ 1, "-A" },
	{ 0, "-c devname" },
	{ 0, "--show-coalesce devname" },
	{ 0, "-C devname adaptive-rx on adaptive-tx off rx-usecs 1 rx-frames 2 rx-usecs-irq 3 rx-frames-irq 4 tx-usecs 5 tx-frames 6 tx-usecs-irq 7 tx-frames-irq 8 stats-block-usecs 9 pkt-rate-low 10" },
	{ 0, "--coalesce devname rx-usecs-low 11 rx-frames-low 12 tx-usecs-low 13 tx-frames-low 14 pkt-rate-high 15 rx-usecs-high 16 rx-frames-high 17 tx-usecs-high 18 tx-frames-high 19 sample-interval 20" },
	{ 1, "-C devname adaptive-rx foo" },
	{ 1, "--coalesce devname adaptive-rx" },
	{ 1, "-C devname foo on" },
	{ 1, "-C" },
	{ 0, "-g devname" },
	{ 0, "--show-ring devname" },
	{ 1, "-g" },
	{ 0, "-G devname rx 1 rx-mini 2 rx-jumbo 3 tx 4" },
	{ 0, "--set-ring devname rx 1 rx-mini 2 rx-jumbo 3 tx 4" },
	{ 1, "-G devname rx foo" },
	{ 1, "--set-ring devname rx" },
	{ 1, "-G devname foo 1" },
	{ 1, "-G" },
	{ 1, "-k" },
	{ 1, "-K" },
	{ 0, "-i devname" },
	{ 0, "--driver devname" },
	{ 1, "-i" },
	{ 0, "-d devname" },
	{ 0, "--register-dump devname raw on file foo" },
	{ 1, "-d devname raw foo" },
	{ 1, "--register-dump devname file" },
	{ 1, "-d devname foo" },
	{ 1, "-d" },
	{ 0, "-e devname" },
	{ 0, "--eeprom-dump devname raw on offset 1 length 2" },
	{ 1, "-e devname raw foo" },
	{ 1, "--eeprom-dump devname offset foo" },
	{ 1, "-e devname length" },
	{ 1, "--eeprom-dump devname foo" },
	{ 1, "-e" },
	{ 0, "-E devname" },
	{ 0, "--change-eeprom devname magic 0x87654321 offset 0 value 1" },
	{ 0, "-E devname magic 0x87654321 offset 0 length 2" },
	{ 1, "-E" },
	{ 0, "-r devname" },
	{ 0, "--negotiate devname" },
	{ 1, "-r" },
	{ 0, "-p devname" },
	{ 0, "--identify devname 1" },
	{ 1, "-p devname 1 foo" },
	{ 1, "--identify devname foo" },
	{ 1, "-p" },
	/* Argument parsing for -t is specialised */
	{ 0, "-t devname" },
	{ 0, "--test devname online" },
	{ 1, "-t devname foo" },
	{ 1, "--test devname online foo" },
	{ 0, "-S devname" },
	{ 0, "--statistics devname" },
	{ 1, "-S" },
	/* Argument parsing for -n/-u is specialised */
	{ 0, "-n devname rx-flow-hash tcp4" },
	{ 0, "-u devname rx-flow-hash sctp4" },
	{ 0, "--show-nfc devname rx-flow-hash udp6" },
	{ 0, "--show-ntuple devname rx-flow-hash esp6" },
	{ 1, "-n devname rx-flow-hash foo" },
	{ 1, "-u devname rx-flow-hash foo" },
	{ 1, "--show-nfc devname rx-flow-hash" },
	{ 1, "--show-ntuple devname rx-flow-hash" },
	{ 1, "-n" },
	/* Argument parsing for -f is specialised */
	{ 1, "-f devname" },
	{ 0, "--flash devname filename" },
	{ 0, "-f devname filename 1" },
	{ 1, "-f devname filename 1 foo" },
	{ 1, "-f" },
	/* Argument parsing for -N/-U is specialised */
	{ 0, "-N devname rx-flow-hash tcp4 mvtsdfn" },
	{ 0, "--config-ntuple devname rx-flow-hash tcp4 r" },
	{ 1, "-U devname rx-flow-hash tcp4" },
	{ 1, "--config-nfc devname rx-flow-hash foo" },
	{ 1, "-N devname rx-flow-hash" },
	{ 1, "--config-ntuple devname foo" },
	{ 0, "-U devname delete 1" },
	{ 1, "--config-nfc devname delete foo" },
	{ 1, "-N devname delete" },
	{ 0, "--config-ntuple devname flow-type ether src 01:23:45:67:89:ab m cd:ef:01:23:45:67 dst 89:ab:cd:ef:01:23 m 45:67:89:ab:cd:ef proto 0x0123 m 0x4567 vlan 0x89ab m 0xcdef action 0" },
	{ 0, "-U devname flow-type ether src 01:23:45:67:89:ab src-mask cd:ef:01:23:45:67 dst 89:ab:cd:ef:01:23 dst-mask 45:67:89:ab:cd:ef proto 0x0123 proto-mask 0x4567 vlan 0x89ab vlan-mask 0xcdef action 1" },
	{ 1, "--config-nfc devname flow-type ether src 01:23:45:67:89: action 3" },
	{ 1, "-N devname flow-type ether src 01:23:45:67:89 action 4" },
	{ 0, "--config-ntuple devname flow-type ip4 src-ip 0.123.45.67 m 89.0.123.45 dst-ip 67.89.0.123 m 45.67.89.0 tos 1 m 1 l4proto 0x23 m 0x45 l4data 0xfedcba98 m 76543210 vlan 0x89ab m 0xcdef action 6" },
	{ 0, "-U devname flow-type ip4 src-ip 0.123.45.67 src-ip-mask 89.0.123.45 dst-ip 67.89.0.123 dst-ip-mask 45.67.89.0 tos 1 tos-mask 1 l4proto 0x23 l4proto-mask 0x45 l4data 0xfedcba98 l4data-mask 76543210 vlan 0x89ab vlan-mask 0xcdef action 7" },
	{ 0, "--config-nfc devname flow-type tcp4 src-ip 0.123.45.67 m 89.0.123.45 dst-ip 67.89.0.123 m 45.67.89.0 tos 1 m 1 src-port 23456 m 7890 dst-port 12345 m 6789 vlan 0x89ab m 0xcdef action 8" },
	{ 0, "-N devname flow-type tcp4 src-ip 0.123.45.67 src-ip-mask 89.0.123.45 dst-ip 67.89.0.123 dst-ip-mask 45.67.89.0 tos 1 tos-mask 1 src-port 23456 src-port-mask 7890 dst-port 12345 dst-port-mask 6789 vlan 0x89ab vlan-mask 0xcdef action 9" },
	{ 0, "--config-ntuple devname flow-type ah4 src-ip 0.123.45.67 m 89.0.123.45 dst-ip 67.89.0.123 m 45.67.89.0 tos 1 m 1 spi 2 m 3 vlan 0x89ab m 0xcdef action 10" },
	{ 0, "-U devname flow-type ah4 src-ip 0.123.45.67 src-ip-mask 89.0.123.45 dst-ip 67.89.0.123 dst-ip-mask 45.67.89.0 tos 1 tos-mask 1 spi 2 spi-mask 3 vlan 0x89ab vlan-mask 0xcdef action 11" },
	{ 1, "--config-nfc devname flow-type tcp4 action foo" },
	{ 1, "-N devname flow-type foo" },
	{ 1, "--config-ntuple devname flow-type" },
	{ 1, "-U devname foo" },
	{ 1, "-N" },
	{ 1, "-U" },
	{ 0, "-T devname" },
	{ 0, "--show-time-stamping devname" },
	{ 1, "-T" },
	{ 0, "-x devname" },
	{ 0, "--show-rxfh-indir devname" },
	{ 0, "--show-rxfh devname" },
	{ 1, "-x" },
	/* Argument parsing for -X is specialised */
	{ 0, "-X devname equal 2" },
	{ 0, "--set-rxfh-indir devname equal 256" },
	{ 1, "-X devname equal 0" },
	{ 1, "--set-rxfh-indir devname equal foo" },
	{ 1, "-X devname equal" },
	{ 1, "-X devname start" },
	{ 1, "-X devname start 3" },
	{ 0, "-X devname start 4 equal 2" },
	{ 0, "--set-rxfh-indir devname weight 1 2 3 4" },
	{ 0, "--set-rxfh-indir devname start 4 weight 1 2 3 4" },
	{ 0, "--rxfh devname hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee" },
	{ 0, "-X devname hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee" },
#if 0
	/* XXX These won't fail as expected because we don't parse the
	 * hash key until after the first send_ioctl().  That needs to
	 * be changed before we enable them.
	 */
	{ 1, "--rxfh devname hkey foo" },
	{ 1, "-X devname hkey foo" },
#endif
	{ 0, "--rxfh devname hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee weight 1 2 3 4" },
	{ 0, "-X devname weight 1 2 3 4 hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee" },
	{ 0, "--rxfh devname hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee equal 2" },
	{ 0, "-X devname equal 2 hkey 48:15:6e:bb:d8:bd:6f:b1:a4:c6:7a:c4:76:1c:29:98:da:e1:ae:6c:2e:12:2f:c0:b9:be:61:3d:00:54:35:9e:09:05:c7:d7:93:72:4a:ee" },
	{ 1, "--rxfh devname weight 1 2 3 4 equal 8" },
	{ 1, "-X devname weight 1 2 3 4 equal 8" },
	{ 1, "-X devname foo" },
	{ 1, "-X" },
	{ 0, "-P devname" },
	{ 0, "--show-permaddr devname" },
	{ 1, "-P" },
	{ 0, "-w devname" },
	{ 0, "--get-dump devname data filename" },
	{ 0, "-w devname data filename" },
	{ 1, "--get-dump devname data" },
	{ 1, "-w devname foo" },
	{ 1, "-w" },
	{ 0, "-W devname 1" },
	{ 0, "--set-dump devname 2" },
	{ 1, "-W devname 1 foo" },
	{ 1, "-W devname foo" },
	{ 1, "-W" },
	{ 0, "-l devname" },
	{ 0, "--show-channels devname" },
	{ 1, "-l" },
	{ 0, "-L devname rx 1 tx 2 other 3 combined 4" },
	{ 0, "--set-channels devname rx 1 tx 2 other 3 combined 4" },
	{ 1, "-L devname rx foo" },
	{ 1, "--set-channels devname rx" },
	{ 0, "-L devname" },
	{ 1, "-L" },
	{ 0, "--show-priv-flags devname" },
	{ 1, "--show-priv-flags devname foo" },
	{ 1, "--show-priv-flags" },
	{ 1, "-m" },
	{ 0, "-m devname" },
	{ 1, "--dump-module-eeprom" },
	{ 0, "--dump-module-eeprom devname" },
	{ 1, "--module-info" },
	{ 0, "--module-info devname" },
	{ 0, "-m devname raw on" },
	{ 0, "-m devname raw off" },
	{ 0, "-m devname hex on" },
	{ 0, "-m devname hex off" },
	{ 1, "-m devname hex on raw on" },
	{ 0, "-m devname offset 4 length 6" },
	{ 1, "--show-eee" },
	{ 0, "--show-eee devname" },
	{ 1, "--show-eee devname foo" },
	{ 1, "--set-eee" },
	{ 1, "--set-eee devname" },
	{ 1, "--set-eee devname foo" },
	{ 0, "--set-eee devname eee on" },
	{ 0, "--set-eee devname eee off" },
	{ 1, "--set-eee devname eee foo" },
	{ 0, "--set-eee devname tx-lpi on" },
	{ 0, "--set-eee devname tx-lpi off" },
	{ 1, "--set-eee devname tx-lpi foo" },
	{ 0, "--set-eee devname tx-timer 42 advertise 0x4321" },
	{ 1, "--set-eee devname tx-timer foo" },
	{ 1, "--set-eee devname advertise foo" },
	{ 1, "--set-fec devname" },
	{ 0, "--set-fec devname encoding auto" },
	{ 0, "--set-fec devname encoding off" },
	{ 0, "--set-fec devname encoding baser rs" },
	{ 0, "--set-fec devname encoding auto auto" },
	/* encoding names are validated by kernel with netlink */
	{ !IS_NL, "--set-fec devname encoding foo" },
	{ !IS_NL, "--set-fec devname encoding auto foo" },
	{ !IS_NL, "--set-fec devname encoding none" },
	{ 1, "--set-fec devname auto" },
	/* can't test --set-priv-flags yet */
	{ 0, "-h" },
	{ 0, "--help" },
	{ 0, "--version" },
	{ 1, "--foo" },
	{ 1, "-foo" },
	{ 1, "-0" },
};

int send_ioctl(struct cmd_context *ctx __maybe_unused, void *cmd __maybe_unused)
{
	/* If we get this far then parsing succeeded */
	test_exit(0);
}

#ifdef ETHTOOL_ENABLE_NETLINK
struct nl_socket;
struct nl_msg_buff;

ssize_t nlsock_sendmsg(struct nl_socket *nlsk __maybe_unused,
		       struct nl_msg_buff *altbuff __maybe_unused)
{
	/* If we get this far then parsing succeeded */
	test_exit(0);
}
#endif

int main(void)
{
	struct test_case *tc;
	int test_rc;
	int rc = 0;

	for (tc = test_cases; tc < test_cases + ARRAY_SIZE(test_cases); tc++) {
		if (getenv("ETHTOOL_TEST_VERBOSE"))
			printf("I: Test command line: ethtool %s\n", tc->args);
		test_rc = test_cmdline(tc->args);
		if (test_rc != tc->rc) {
			fprintf(stderr, "E: ethtool %s returns %d\n",
				tc->args, test_rc);
			rc = 1;
		}
	}

	return rc;
}
