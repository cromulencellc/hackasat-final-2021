#ifdef __rtems__
#define __need_getopt_newlib
#include <getopt.h>
#endif
/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Muuss.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if !__rtems__
#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1989, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)ping.c	8.1 (Berkeley) 6/5/93";
#endif /* not lint */
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 *			P I N G . C
 *
 * Using the Internet Control Message Protocol (ICMP) "ECHO" facility,
 * measure round-trip-delays and packet loss across network paths.
 *
 * Author -
 *	Mike Muuss
 *	U. S. Army Ballistic Research Laboratory
 *	December, 1983
 *
 * Status -
 *	Public Domain.  Distribution Unlimited.
 * Bugs -
 *	More statistics could always be gathered.
 *	This program has to run SUID to ROOT to access the ICMP socket.
 */

#include <sys/param.h>		/* NB: we rely on this for <sys/types.h> */
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <arpa/inet.h>

#ifdef IPSEC
#include <netipsec/ipsec.h>
#endif /*IPSEC*/

#include <ctype.h>
//#include "err.h"
#include <errno.h>
#if !defined(__rtems__)
#include <math.h>
#endif
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sysexits.h>
#include <unistd.h>

#include "err.h"
#include "sysexits.h"
#include <sys/select.h>
#ifdef __rtems__
#include <rtems/shell.h>
#endif /* __rtems__ */

#define	INADDR_LEN	((int)sizeof(in_addr_t))
#define	TIMEVAL_LEN	((int)sizeof(struct tv32))
#define	MASK_LEN	(ICMP_MASKLEN - ICMP_MINLEN)
#define	TS_LEN		(ICMP_TSLEN - ICMP_MINLEN)
#define	DEFDATALEN	56		/* default data length */
#define	FLOOD_BACKOFF	20000		/* usecs to back off if F_FLOOD mode */
					/* runs out of buffer space */
#define	MAXIPLEN	(sizeof(struct ip) + MAX_IPOPTLEN)
#define	MAXICMPLEN	(ICMP_ADVLENMIN + MAX_IPOPTLEN)
#define	MAXWAIT		10000		/* max ms to wait for response */
#define	MAXALARM	(60 * 60)	/* max seconds for alarm timeout */
#define	MAXTOS		255

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~B(bit)))
#define	TST(bit)	(A(bit) & B(bit))

struct tv32 {
	int32_t tv32_sec;
	int32_t tv32_usec;
};

/* various options */
#if !__rtems__
int options;
#endif
#define	F_FLOOD		0x0001
#define	F_INTERVAL	0x0002
#define	F_NUMERIC	0x0004
#define	F_PINGFILLED	0x0008
#define	F_QUIET		0x0010
#define	F_RROUTE	0x0020
#define	F_SO_DEBUG	0x0040
#define	F_SO_DONTROUTE	0x0080
#define	F_VERBOSE	0x0100
#define	F_QUIET2	0x0200
#define	F_NOLOOP	0x0400
#define	F_MTTL		0x0800
#define	F_MIF		0x1000
#define	F_AUDIBLE	0x2000
#ifdef IPSEC
#ifdef IPSEC_POLICY_IPSEC
#define F_POLICY	0x4000
#endif /*IPSEC_POLICY_IPSEC*/
#endif /*IPSEC*/
#define	F_TTL		0x8000
#define	F_MISSED	0x10000
#define	F_ONCE		0x20000
#define	F_HDRINCL	0x40000
#define	F_MASK		0x80000
#define	F_TIME		0x100000
#define	F_SWEEP		0x200000
#define	F_WAITTIME	0x400000

/*
 * MAX_DUP_CHK is the number of bits in received table, i.e. the maximum
 * number of received sequence numbers we can keep track of.  Change 128
 * to 8192 for complete accuracy...
 */
#define	MAX_DUP_CHK	(8 * 128)
#if !__rtems__
int mx_dup_ck = MAX_DUP_CHK;
char rcvd_tbl[MAX_DUP_CHK / 8];

struct sockaddr_in whereto;	/* who to ping */
int datalen = DEFDATALEN;
int maxpayload;
int s;				/* socket file descriptor */
u_char outpackhdr[IP_MAXPACKET], *outpack;
#endif
char BBELL = '\a';		/* characters written for MISSED and AUDIBLE */
char BSPACE = '\b';		/* characters written for flood */
char DOT = '.';
#if !__rtems__
char *hostname;
char *shostname;
int ident;			/* process id to identify our packets */
int uid;			/* cached uid for micro-optimization */
u_char icmp_type = ICMP_ECHO;
u_char icmp_type_rsp = ICMP_ECHOREPLY;
int phdr_len = 0;
int send_len;

/* counters */
long nmissedmax;		/* max value of ntransmitted - nreceived - 1 */
long npackets;			/* max packets to transmit */
long nreceived;			/* # of packets we got back */
long nrepeats;			/* number of duplicates */
long ntransmitted;		/* sequence # for outbound packets = #sent */
long snpackets;			/* max packets to transmit in one sweep */
long snreceived;		/* # of packets we got back in this sweep */
long sntransmitted;		/* # of packets we sent in this sweep */
int sweepmax;			/* max value of payload in sweep */
int sweepmin = 0;		/* start value of payload in sweep */
int sweepincr = 1;		/* payload increment in sweep */
int interval = 1000;		/* interval between packets, ms */
int waittime = MAXWAIT;		/* timeout for each packet */
long nrcvtimeout = 0;		/* # of packets we got back after waittime */

/* timing */
int timing;			/* flag to do timing */
double tmin = 999999999.0;	/* minimum round trip time */
double tmax = 0.0;		/* maximum round trip time */
double tsum = 0.0;		/* sum of all times, for doing average */
double tsumsq = 0.0;		/* sum of all times squared, for std. dev. */

volatile sig_atomic_t finish_up;  /* nonzero if we've been told to finish up */
volatile sig_atomic_t siginfo_p;

static void fill(char *, char *);
static u_short in_cksum(u_short *, int);
static void check_status(void);
static void finish(void) __dead2;
static void pinger(void);
static char *pr_addr(struct in_addr);
static char *pr_ntime(n_time);
static void pr_icmph(struct icmp *);
static void pr_iph(struct ip *);
static void pr_pack(char *, int, struct sockaddr_in *, struct timeval *);
static void pr_retip(struct ip *);
static void status(int);
static void stopit(int);
static void tvsub(struct timeval *, struct timeval *);
static void usage(void) __dead2;
#endif

#if __rtems__
#define CMSG_SPACE(l)   (_ALIGN(sizeof(struct cmsghdr)) + _ALIGN(l))
#define CMSG_LEN(l)     (_ALIGN(sizeof(struct cmsghdr)) + (l))
typedef struct
{
  int options;
  int mx_dup_ck;
  char rcvd_tbl[MAX_DUP_CHK / 8];
  struct sockaddr_in whereto;
  int datalen;
  int maxpayload;
  int s;
  u_char outpackhdr[IP_MAXPACKET];
  u_char *outpack;
  char *hostname;
  char *shostname;
  int ident;
  int uid;
  u_char icmp_type;
  u_char icmp_type_rsp;
  int phdr_len;
  int send_len;
  long nmissedmax;
  long npackets;
  long nreceived;
  long nrepeats;
  long ntransmitted;
  long snpackets;
  long snreceived;
  long sntransmitted;
  int sweepmax;
  int sweepmin;
  int sweepincr;
  int interval;
  int waittime;
  long nrcvtimeout;
  int timing;
  double tmin;
  double tmax;
  double tsum;
  double tsumsq;
  volatile sig_atomic_t finish_up;
  volatile sig_atomic_t siginfo_p;

  /* main */
  u_char packet[IP_MAXPACKET] __aligned(4);

  /* pr_pack */
	int old_rrlen;
	char old_rr[MAX_IPOPTLEN];

  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_globals_t;

#define options_ globals->options
#define mx_dup_ck globals->mx_dup_ck
#define rcvd_tbl globals->rcvd_tbl
#define whereto globals->whereto
#define datalen globals->datalen
#define maxpayload globals->maxpayload
#define s globals->s
#define outpackhdr globals->outpackhdr
#define outpack globals->outpack
#define hostname globals->hostname
#define shostname globals->shostname
#define ident globals->ident
#define uid globals->uid
#define icmp_type_ globals->icmp_type
#define icmp_type_rsp globals->icmp_type_rsp
#define phdr_len globals->phdr_len
#define send_len globals->send_len
#define nmissedmax globals->nmissedmax
#define npackets globals->npackets
#define nreceived globals->nreceived
#define nrepeats globals->nrepeats
#define ntransmitted globals->ntransmitted
#define snpackets globals->snpackets
#define snreceived globals->snreceived
#define sntransmitted globals->sntransmitted
#define sweepmax globals->sweepmax
#define sweepmin globals->sweepmin
#define sweepincr globals->sweepincr
#define interval globals->interval
#define waittime globals->waittime
#define nrcvtimeout globals->nrcvtimeout
#define timing globals->timing
#define tmin globals->tmin
#define tmax globals->tmax
#define tsum globals->tsum
#define tsumsq globals->tsumsq
#define finish_up globals->finish_up
#define siginfo_p globals->siginfo_p

#define old_rrlen globals->old_rrlen
#define old_rr globals->old_rr

#define packet_ globals->packet

static u_short in_cksum(u_short *, int);
static char *pr_ntime(n_time);
static void pr_icmph(struct icmp *);
static void pr_iph(struct ip *);
static void pr_retip(struct ip *);
static void stopit(int);
static void tvsub(struct timeval *, struct timeval *);

#define fill(_a1, _a2)  g_fill(_a1, _a2, globals)
static void g_fill(char *_a1, char *_a2, rtems_shell_globals_t* globals);

#define check_status() g_check_status(globals)
static void g_check_status(rtems_shell_globals_t* globals);

#define finish() g_finish(globals)
static void g_finish(rtems_shell_globals_t* globals) __dead2;

#define pinger() g_pinger(globals)
static void g_pinger(rtems_shell_globals_t* globals);

#define pr_addr(_a1) g_pr_addr(_a1, globals)
static char *g_pr_addr(struct in_addr, rtems_shell_globals_t* globals);

#define pr_pack(_a1, _a2, _a3, _a4) g_pr_pack(_a1, _a2, _a3, _a4, globals)
static void g_pr_pack(char *, int, struct sockaddr_in *, struct timeval *, rtems_shell_globals_t* globals);

#define usage() g_usage(globals)
static void g_usage(rtems_shell_globals_t* globals) __dead2;

static void
rtems_shell_ping_exit (rtems_shell_globals_t* globals, int code)
{
  globals->exit_code = code;
  longjmp (globals->exit_jmp, 1);
}

#define exit(_c) rtems_shell_ping_exit (globals, _c)
#define _exit(_c) exit(_c)

static int main_ping(int argc, char *const *argv, rtems_shell_globals_t* globals);
static int rtems_shell_main_ping(int argc, char *argv[])
{
  rtems_shell_globals_t* globals = malloc(sizeof(rtems_shell_globals_t));
  if (!globals)
  {
    printf("error: no memory\n");
    return 1;
  }
  memset (globals, 0, sizeof (rtems_shell_globals_t));
  npackets = 5;
  datalen = DEFDATALEN;
  icmp_type_ = ICMP_ECHO;
  icmp_type_rsp = ICMP_ECHOREPLY;
  phdr_len = 0;
  sweepmin = 0;
  sweepincr = 1;
  interval = 1000;
  waittime = MAXWAIT;
  nrcvtimeout = 0;
  tmin = 999999999.0;
  tmax = 0.0;
  tsum = 0.0;
  tsumsq = 0.0;
  globals->exit_code = 1;
  if (setjmp (globals->exit_jmp) == 0)
    return main_ping (argc, argv, globals);
  return globals->exit_code;
}
#endif



int
#ifdef __rtems__
main_ping(argc, argv, globals)
#else
main(argc, argv)
#endif
	int argc;
	char *const *argv;
  rtems_shell_globals_t* globals;
{
	struct sockaddr_in from, sock_in;
	struct in_addr ifaddr;
	struct timeval last, intvl;
	struct iovec iov;
	struct ip *ip;
	struct msghdr msg;
	struct sigaction si_sa;
	size_t sz;
#if !__rtems__
	u_char *datap, packet[IP_MAXPACKET] __aligned(4);
#else
	u_char *datap;
#endif
	char *ep, *source, *target, *payload;
	struct hostent *hp;
#ifdef IPSEC_POLICY_IPSEC
	char *policy_in, *policy_out;
#endif
	struct sockaddr_in *to;
	double t;
	u_long alarmtimeout, ultmp;
	int almost_done, ch, df, hold, i, icmp_len, mib[4], preload, sockerrno,
	    tos, ttl;
	char ctrl[CMSG_SPACE(sizeof(struct timeval))];
	char hnamebuf[MAXHOSTNAMELEN], snamebuf[MAXHOSTNAMELEN];
#ifdef IP_OPTIONS
	char rspace[MAX_IPOPTLEN];	/* record route space */
#endif
	unsigned char loop, mttl;
#ifdef __rtems__
	struct getopt_data getopt_reent;
#define optarg getopt_reent.optarg
#define optind getopt_reent.optind
#define opterr getopt.reent.opterr
#define optopt getopt.reent.optopt
#endif

	payload = source = NULL;
#ifdef IPSEC_POLICY_IPSEC
	policy_in = policy_out = NULL;
#endif

	/*
	 * Do the stuff that we need root priv's for *first*, and
	 * then drop our setuid bit.  Save error reporting for
	 * after arg parsing.
	 */
	s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	sockerrno = errno;

	setuid(getuid());
	uid = getuid();

	alarmtimeout = df = preload = tos = 0;

	outpack = outpackhdr + sizeof(struct ip);
#ifdef __rtems__
	memset(&getopt_reent, 0, sizeof(getopt_data));
	while ((ch = getopt_r(argc, argv,
#else
	while ((ch = getopt(argc, argv,
#endif
		"Aac:DdfG:g:h:I:i:Ll:M:m:nop:QqRrS:s:T:t:vW:z:"
#ifdef IPSEC
#ifdef IPSEC_POLICY_IPSEC
		"P:"
#endif /*IPSEC_POLICY_IPSEC*/
#endif /*IPSEC*/
#ifdef __rtems__
		, &getopt_reent
#endif
		)) != -1)
	{
		switch(ch) {
		case 'A':
			options_ |= F_MISSED;
			break;
		case 'a':
			options_ |= F_AUDIBLE;
			break;
		case 'c':
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp > LONG_MAX || !ultmp)
				errx(&globals->exit_jmp, EX_USAGE,
				    "invalid count of packets to transmit: `%s'",
				    optarg);
			npackets = ultmp;
			break;
		case 'D':
			options_ |= F_HDRINCL;
			df = 1;
			break;
		case 'd':
			options_ |= F_SO_DEBUG;
			break;
		case 'f':
			if (uid) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM, "-f flag");
			}
			options_ |= F_FLOOD;
			setbuf(stdout, (char *)NULL);
			break;
		case 'G': /* Maximum packet size for ping sweep */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg)
				errx(&globals->exit_jmp, EX_USAGE, "invalid packet size: `%s'",
				    optarg);
			if (uid != 0 && ultmp > DEFDATALEN) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM,
				    "packet size too large: %lu > %u",
				    ultmp, DEFDATALEN);
			}
			options_ |= F_SWEEP;
			sweepmax = ultmp;
			break;
		case 'g': /* Minimum packet size for ping sweep */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg)
				errx(&globals->exit_jmp, EX_USAGE, "invalid packet size: `%s'",
				    optarg);
			if (uid != 0 && ultmp > DEFDATALEN) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM,
				    "packet size too large: %lu > %u",
				    ultmp, DEFDATALEN);
			}
			options_ |= F_SWEEP;
			sweepmin = ultmp;
			break;
		case 'h': /* Packet size increment for ping sweep */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp < 1)
				errx(&globals->exit_jmp, EX_USAGE, "invalid increment size: `%s'",
				    optarg);
			if (uid != 0 && ultmp > DEFDATALEN) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM,
				    "packet size too large: %lu > %u",
				    ultmp, DEFDATALEN);
			}
			options_ |= F_SWEEP;
			sweepincr = ultmp;
			break;
		case 'I':		/* multicast interface */
			if (inet_aton(optarg, &ifaddr) == 0)
				errx(&globals->exit_jmp, EX_USAGE,
				    "invalid multicast interface: `%s'",
				    optarg);
			options_ |= F_MIF;
			break;
		case 'i':		/* wait between sending packets */
			t = strtod(optarg, &ep) * 1000.0;
			if (*ep || ep == optarg || t > (double)INT_MAX)
				errx(&globals->exit_jmp, EX_USAGE, "invalid timing interval: `%s'",
				    optarg);
			options_ |= F_INTERVAL;
			interval = (int)t;
			if (uid && interval < 1000) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM, "-i interval too short");
			}
			break;
		case 'L':
			options_ |= F_NOLOOP;
			loop = 0;
			break;
		case 'l':
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp > INT_MAX)
				errx(&globals->exit_jmp, EX_USAGE,
				    "invalid preload value: `%s'", optarg);
			if (uid) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM, "-l flag");
			}
			preload = ultmp;
			break;
		case 'M':
			switch(optarg[0]) {
			case 'M':
			case 'm':
				options_ |= F_MASK;
				break;
			case 'T':
			case 't':
				options_ |= F_TIME;
				break;
			default:
				errx(&globals->exit_jmp, EX_USAGE, "invalid message: `%c'", optarg[0]);
				break;
			}
			break;
		case 'm':		/* TTL */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp > MAXTTL)
				errx(&globals->exit_jmp, EX_USAGE, "invalid TTL: `%s'", optarg);
			ttl = ultmp;
			options_ |= F_TTL;
			break;
		case 'n':
			options_ |= F_NUMERIC;
			break;
		case 'o':
			options_ |= F_ONCE;
			break;
#ifdef IPSEC
#ifdef IPSEC_POLICY_IPSEC
		case 'P':
			options_ |= F_POLICY;
			if (!strncmp("in", optarg, 2))
				policy_in = strdup(optarg);
			else if (!strncmp("out", optarg, 3))
				policy_out = strdup(optarg);
			else
				errx(&globals->exit_jmp, 1, "invalid security policy");
			break;
#endif /*IPSEC_POLICY_IPSEC*/
#endif /*IPSEC*/
		case 'p':		/* fill buffer with user pattern */
			options_ |= F_PINGFILLED;
			payload = optarg;
			break;
		case 'Q':
			options_ |= F_QUIET2;
			break;
		case 'q':
			options_ |= F_QUIET;
			break;
		case 'R':
			options_ |= F_RROUTE;
			break;
		case 'r':
			options_ |= F_SO_DONTROUTE;
			break;
		case 'S':
			source = optarg;
			break;
		case 's':		/* size of packet to send */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg)
				errx(&globals->exit_jmp, EX_USAGE, "invalid packet size: `%s'",
				    optarg);
			if (uid != 0 && ultmp > DEFDATALEN) {
				errno = EPERM;
				err(&globals->exit_jmp, EX_NOPERM,
				    "packet size too large: %lu > %u",
				    ultmp, DEFDATALEN);
			}
			datalen = ultmp;
			break;
		case 'T':		/* multicast TTL */
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp > MAXTTL)
				errx(&globals->exit_jmp, EX_USAGE, "invalid multicast TTL: `%s'",
				    optarg);
			mttl = ultmp;
			options_ |= F_MTTL;
			break;
		case 't':
			alarmtimeout = strtoul(optarg, &ep, 0);
			if ((alarmtimeout < 1) || (alarmtimeout == ULONG_MAX))
				errx(&globals->exit_jmp, EX_USAGE, "invalid timeout: `%s'",
				    optarg);
			if (alarmtimeout > MAXALARM)
				errx(&globals->exit_jmp, EX_USAGE, "invalid timeout: `%s' > %d",
				    optarg, MAXALARM);
			alarm((int)alarmtimeout);
			break;
		case 'v':
			options_ |= F_VERBOSE;
			break;
		case 'W':		/* wait ms for answer */
			t = strtod(optarg, &ep);
			if (*ep || ep == optarg || t > (double)INT_MAX)
				errx(&globals->exit_jmp, EX_USAGE, "invalid timing interval: `%s'",
				    optarg);
			options_ |= F_WAITTIME;
			waittime = (int)t;
			break;
		case 'z':
			options_ |= F_HDRINCL;
			ultmp = strtoul(optarg, &ep, 0);
			if (*ep || ep == optarg || ultmp > MAXTOS)
				errx(&globals->exit_jmp, EX_USAGE, "invalid TOS: `%s'", optarg);
			tos = ultmp;
			break;
		default:
			usage();

		}
	}

	if (argc - optind != 1)
		usage();

	target = argv[optind];

	switch (options_ & (F_MASK|F_TIME)) {
	case 0: break;
	case F_MASK:
		icmp_type_ = ICMP_MASKREQ;
		icmp_type_rsp = ICMP_MASKREPLY;
		phdr_len = MASK_LEN;
		if (!(options_ & F_QUIET))
			(void)printf("ICMP_MASKREQ\n");
		break;
	case F_TIME:
		icmp_type_ = ICMP_TSTAMP;
		icmp_type_rsp = ICMP_TSTAMPREPLY;
		phdr_len = TS_LEN;
		if (!(options_ & F_QUIET))
			(void)printf("ICMP_TSTAMP\n");
		break;
	default:
		errx(&globals->exit_jmp, EX_USAGE, "ICMP_TSTAMP and ICMP_MASKREQ are exclusive.");
		break;
	}
	icmp_len = sizeof(struct ip) + ICMP_MINLEN + phdr_len;
	if (options_ & F_RROUTE)
		icmp_len += MAX_IPOPTLEN;
	maxpayload = IP_MAXPACKET - icmp_len;
	if (datalen > maxpayload)
		errx(&globals->exit_jmp, EX_USAGE, "packet size too large: %d > %d", datalen,
		    maxpayload);
	send_len = icmp_len + datalen;
	datap = &outpack[ICMP_MINLEN + phdr_len + TIMEVAL_LEN];
	if (options_ & F_PINGFILLED) {
		fill((char *)datap, payload);
	}
	if (source) {
		bzero((char *)&sock_in, sizeof(sock_in));
		sock_in.sin_family = AF_INET;
		if (inet_aton(source, &sock_in.sin_addr) != 0) {
			shostname = source;
		} else {
			hp = gethostbyname2(source, AF_INET);
			if (!hp)
				errx(&globals->exit_jmp, EX_NOHOST, "cannot resolve %s: %s",
				    source, hstrerror(h_errno));

			sock_in.sin_len = sizeof sock_in;
			if ((unsigned)hp->h_length > sizeof(sock_in.sin_addr) ||
			    hp->h_length < 0)
				errx(&globals->exit_jmp, 1, "gethostbyname2: illegal address");
			memcpy(&sock_in.sin_addr, hp->h_addr_list[0],
			    sizeof(sock_in.sin_addr));
			(void)strncpy(snamebuf, hp->h_name,
			    sizeof(snamebuf) - 1);
			snamebuf[sizeof(snamebuf) - 1] = '\0';
			shostname = snamebuf;
		}
		if (bind(s, (struct sockaddr *)&sock_in, sizeof sock_in) == -1)
			err(&globals->exit_jmp, 1, "bind");
	}

	bzero(&whereto, sizeof(whereto));
	to = &whereto;
	to->sin_family = AF_INET;
	to->sin_len = sizeof *to;
	if (inet_aton(target, &to->sin_addr) != 0) {
		hostname = target;
	} else {
		hp = gethostbyname2(target, AF_INET);
		if (!hp)
			errx(&globals->exit_jmp, EX_NOHOST, "cannot resolve %s: %s",
			    target, hstrerror(h_errno));

		if ((unsigned)hp->h_length > sizeof(to->sin_addr))
			errx(&globals->exit_jmp, 1, "gethostbyname2 returned an illegal address");
		memcpy(&to->sin_addr, hp->h_addr_list[0], sizeof to->sin_addr);
		(void)strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
		hnamebuf[sizeof(hnamebuf) - 1] = '\0';
		hostname = hnamebuf;
	}

	if (options_ & F_FLOOD && options_ & F_INTERVAL)
		errx(&globals->exit_jmp, EX_USAGE, "-f and -i: incompatible options");

	if (options_ & F_FLOOD && IN_MULTICAST(ntohl(to->sin_addr.s_addr)))
		errx(&globals->exit_jmp, EX_USAGE,
		    "-f flag cannot be used with multicast destination");
	if (options_ & (F_MIF | F_NOLOOP | F_MTTL)
	    && !IN_MULTICAST(ntohl(to->sin_addr.s_addr)))
		errx(&globals->exit_jmp, EX_USAGE,
		    "-I, -L, -T flags cannot be used with unicast destination");

	if (datalen >= TIMEVAL_LEN)	/* can we time transfer */
		timing = 1;

	if (!(options_ & F_PINGFILLED))
		for (i = TIMEVAL_LEN; i < datalen; ++i)
			*datap++ = i;

	ident = getpid() & 0xFFFF;

	if (s < 0) {
		errno = sockerrno;
		err(&globals->exit_jmp, EX_OSERR, "socket");
	}
	hold = 1;
	if (options_ & F_SO_DEBUG)
		(void)setsockopt(s, SOL_SOCKET, SO_DEBUG, (char *)&hold,
		    sizeof(hold));
	if (options_ & F_SO_DONTROUTE)
		(void)setsockopt(s, SOL_SOCKET, SO_DONTROUTE, (char *)&hold,
		    sizeof(hold));
#ifdef IPSEC
#ifdef IPSEC_POLICY_IPSEC
	if (options_ & F_POLICY) {
		char *buf;
		if (policy_in != NULL) {
			buf = ipsec_set_policy(policy_in, strlen(policy_in));
			if (buf == NULL)
				errx(&globals->exit_jmp, EX_CONFIG, "%s", ipsec_strerror());
			if (setsockopt(s, IPPROTO_IP, IP_IPSEC_POLICY,
					buf, ipsec_get_policylen(buf)) < 0)
				err(EX_CONFIG,
				    "ipsec policy cannot be configured");
			free(buf);
		}

		if (policy_out != NULL) {
			buf = ipsec_set_policy(policy_out, strlen(policy_out));
			if (buf == NULL)
				errx(&globals->exit_jmp, EX_CONFIG, "%s", ipsec_strerror());
			if (setsockopt(s, IPPROTO_IP, IP_IPSEC_POLICY,
					buf, ipsec_get_policylen(buf)) < 0)
				err(EX_CONFIG,
				    "ipsec policy cannot be configured");
			free(buf);
		}
	}
#endif /*IPSEC_POLICY_IPSEC*/
#endif /*IPSEC*/

	if (options_ & F_HDRINCL) {
		ip = (struct ip*)outpackhdr;
		if (!(options_ & (F_TTL | F_MTTL))) {
			mib[0] = CTL_NET;
			mib[1] = PF_INET;
			mib[2] = IPPROTO_IP;
			mib[3] = IPCTL_DEFTTL;
			sz = sizeof(ttl);
			if (sysctl(mib, 4, &ttl, &sz, NULL, 0) == -1)
				err(&globals->exit_jmp, 1, "sysctl(net.inet.ip.ttl)");
		}
		setsockopt(s, IPPROTO_IP, IP_HDRINCL, &hold, sizeof(hold));
		ip->ip_v = IPVERSION;
		ip->ip_hl = sizeof(struct ip) >> 2;
		ip->ip_tos = tos;
		ip->ip_id = 0;
		ip->ip_off = df ? IP_DF : 0;
		ip->ip_ttl = ttl;
		ip->ip_p = IPPROTO_ICMP;
		ip->ip_src.s_addr = source ? sock_in.sin_addr.s_addr : INADDR_ANY;
		ip->ip_dst = to->sin_addr;
        }
	/* record route option */
	if (options_ & F_RROUTE) {
#ifdef IP_OPTIONS
		bzero(rspace, sizeof(rspace));
		rspace[IPOPT_OPTVAL] = IPOPT_RR;
		rspace[IPOPT_OLEN] = sizeof(rspace) - 1;
		rspace[IPOPT_OFFSET] = IPOPT_MINOFF;
		rspace[sizeof(rspace) - 1] = IPOPT_EOL;
		if (setsockopt(s, IPPROTO_IP, IP_OPTIONS, rspace,
		    sizeof(rspace)) < 0)
			err(&globals->exit_jmp, EX_OSERR, "setsockopt IP_OPTIONS");
#else
		errx(&globals->exit_jmp, EX_UNAVAILABLE,
		    "record route not available in this implementation");
#endif /* IP_OPTIONS */
	}

	if (options_ & F_TTL) {
		if (setsockopt(s, IPPROTO_IP, IP_TTL, &ttl,
		    sizeof(ttl)) < 0) {
			err(&globals->exit_jmp, EX_OSERR, "setsockopt IP_TTL");
		}
	}
	if (options_ & F_NOLOOP) {
		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,
		    sizeof(loop)) < 0) {
			err(&globals->exit_jmp, EX_OSERR, "setsockopt IP_MULTICAST_LOOP");
		}
	}
	if (options_ & F_MTTL) {
		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &mttl,
		    sizeof(mttl)) < 0) {
			err(&globals->exit_jmp, EX_OSERR, "setsockopt IP_MULTICAST_TTL");
		}
	}
	if (options_ & F_MIF) {
		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, &ifaddr,
		    sizeof(ifaddr)) < 0) {
			err(&globals->exit_jmp, EX_OSERR, "setsockopt IP_MULTICAST_IF");
		}
	}
#ifdef SO_TIMESTAMP
	{ int on = 1;
	if (setsockopt(s, SOL_SOCKET, SO_TIMESTAMP, &on, sizeof(on)) < 0)
		err(&globals->exit_jmp, EX_OSERR, "setsockopt SO_TIMESTAMP");
	}
#endif
	if (sweepmax) {
		if (sweepmin >= sweepmax)
			errx(&globals->exit_jmp, EX_USAGE, "Maximum packet size must be greater than the minimum packet size");

		if (datalen != DEFDATALEN)
			errx(&globals->exit_jmp, EX_USAGE, "Packet size and ping sweep are mutually exclusive");

		if (npackets > 0) {
			snpackets = npackets;
			npackets = 0;
		} else
			snpackets = 1;
		datalen = sweepmin;
		send_len = icmp_len + sweepmin;
	}
	if (options_ & F_SWEEP && !sweepmax)
		errx(&globals->exit_jmp, EX_USAGE, "Maximum sweep size must be specified");

	/*
	 * When pinging the broadcast address, you can get a lot of answers.
	 * Doing something so evil is useful if you are trying to stress the
	 * ethernet, or just want to fill the arp cache to get some stuff for
	 * /etc/ethers.  But beware: RFC 1122 allows hosts to ignore broadcast
	 * or multicast pings if they wish.
	 */

	/*
	 * XXX receive buffer needs undetermined space for mbuf overhead
	 * as well.
	 */
	hold = IP_MAXPACKET + 128;
	(void)setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&hold,
	    sizeof(hold));
	if (uid == 0)
		(void)setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&hold,
		    sizeof(hold));

	if (to->sin_family == AF_INET) {
		(void)printf("PING %s (%s)", hostname,
		    inet_ntoa(to->sin_addr));
		if (source)
			(void)printf(" from %s", shostname);
		if (sweepmax)
			(void)printf(": (%d ... %d) data bytes\n",
			    sweepmin, sweepmax);
		else
			(void)printf(": %d data bytes\n", datalen);

	} else {
		if (sweepmax)
			(void)printf("PING %s: (%d ... %d) data bytes\n",
			    hostname, sweepmin, sweepmax);
		else
			(void)printf("PING %s: %d data bytes\n", hostname, datalen);
	}

	/*
	 * Use sigaction() instead of signal() to get unambiguous semantics,
	 * in particular with SA_RESTART not set.
	 */

	sigemptyset(&si_sa.sa_mask);
	si_sa.sa_flags = 0;

	si_sa.sa_handler = stopit;
	if (sigaction(SIGINT, &si_sa, 0) == -1) {
		err(&globals->exit_jmp, EX_OSERR, "sigaction SIGINT");
	}

#ifdef SIGINFO
	si_sa.sa_handler = status;
	if (sigaction(SIGINFO, &si_sa, 0) == -1) {
		err(EX_OSERR, "sigaction");
	}
#endif

        if (alarmtimeout > 0) {
		si_sa.sa_handler = stopit;
		if (sigaction(SIGALRM, &si_sa, 0) == -1)
			err(&globals->exit_jmp, EX_OSERR, "sigaction SIGALRM");
        }

	bzero(&msg, sizeof(msg));
	msg.msg_name = (caddr_t)&from;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
#ifdef SO_TIMESTAMP
	msg.msg_control = (caddr_t)ctrl;
#endif
	iov.iov_base = packet_;
	iov.iov_len = IP_MAXPACKET;

	if (preload == 0)
		pinger();		/* send the first ping */
	else {
		if (npackets != 0 && preload > npackets)
			preload = npackets;
		while (preload--)	/* fire off them quickies */
			pinger();
	}
	(void)gettimeofday(&last, NULL);

	if (options_ & F_FLOOD) {
		intvl.tv_sec = 0;
		intvl.tv_usec = 10000;
	} else {
		intvl.tv_sec = interval / 1000;
		intvl.tv_usec = interval % 1000 * 1000;
	}

	almost_done = 0;
	while (!finish_up) {
		struct timeval now, timeout;
		fd_set rfds;
		int cc, n;

		check_status();
		if ((unsigned)s >= FD_SETSIZE)
			errx(&globals->exit_jmp, EX_OSERR, "descriptor too large");
		FD_ZERO(&rfds);
		FD_SET(s, &rfds);
		(void)gettimeofday(&now, NULL);
		timeout.tv_sec = last.tv_sec + intvl.tv_sec - now.tv_sec;
		timeout.tv_usec = last.tv_usec + intvl.tv_usec - now.tv_usec;
		while (timeout.tv_usec < 0) {
			timeout.tv_usec += 1000000;
			timeout.tv_sec--;
		}
		while (timeout.tv_usec >= 1000000) {
			timeout.tv_usec -= 1000000;
			timeout.tv_sec++;
		}
		if (timeout.tv_sec < 0)
			timeout.tv_sec = timeout.tv_usec = 0;
		n = select(s + 1, &rfds, NULL, NULL, &timeout);
		if (n < 0)
			continue;	/* Must be EINTR. */
		if (n == 1) {
			struct timeval *tv = NULL;
#ifdef SO_TIMESTAMP
			struct cmsghdr *cmsg = (struct cmsghdr *)&ctrl;

			msg.msg_controllen = sizeof(ctrl);
#endif

	bzero(&msg, sizeof(msg));
	msg.msg_name = (caddr_t)&from;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
#ifdef SO_TIMESTAMP
	msg.msg_control = (caddr_t)ctrl;
#endif
	iov.iov_base = packet_;
	iov.iov_len = IP_MAXPACKET;

			msg.msg_namelen = sizeof(from);
			if ((cc = recvmsg(s, &msg, 0)) < 0) {
				if (errno == EINTR)
					continue;
				warn("recvmsg");
				continue;
			}
#ifdef SO_TIMESTAMP
			if (cmsg->cmsg_level == SOL_SOCKET &&
			    cmsg->cmsg_type == SCM_TIMESTAMP &&
			    cmsg->cmsg_len == CMSG_LEN(sizeof *tv)) {
				/* Copy to avoid alignment problems: */
				memcpy(&now, CMSG_DATA(cmsg), sizeof(now));
				tv = &now;
			}
#endif
			if (tv == NULL) {
				(void)gettimeofday(&now, NULL);
				tv = &now;
			}
			pr_pack((char *)packet_, cc, &from, tv);
			if ((options_ & F_ONCE && nreceived) ||
			    (npackets && nreceived >= npackets))
				break;
		}
		if (n == 0 || options_ & F_FLOOD) {
			if (sweepmax && sntransmitted == snpackets) {
				for (i = 0; i < sweepincr ; ++i)
					*datap++ = i;
				datalen += sweepincr;
				if (datalen > sweepmax)
					break;
				send_len = icmp_len + datalen;
				sntransmitted = 0;
			}
			if (!npackets || ntransmitted < npackets)
				pinger();
			else {
				if (almost_done)
					break;
				almost_done = 1;
				intvl.tv_usec = 0;
				if (nreceived) {
					intvl.tv_sec = 2 * tmax / 1000;
					if (!intvl.tv_sec)
						intvl.tv_sec = 1;
				} else {
					intvl.tv_sec = waittime / 1000;
					intvl.tv_usec = waittime % 1000 * 1000;
				}
			}
			(void)gettimeofday(&last, NULL);
			if (ntransmitted - nreceived - 1 > nmissedmax) {
				nmissedmax = ntransmitted - nreceived - 1;
				if (options_ & F_MISSED)
					(void)write(STDOUT_FILENO, &BBELL, 1);
			}
		}
	}
	finish();
#ifdef __rtems__
	/* RTEMS shell programs return -- they do not exit */
	if (nreceived)
		return(0);
	else
		return(2);
#endif
	/* NOTREACHED */
	exit(0);	/* Make the compiler happy */
}

/*
 * stopit --
 *	Set the global bit that causes the main loop to quit.
 * Do NOT call finish() from here, since finish() does far too much
 * to be called from a signal handler.
 */
void
stopit(sig)
	int sig __unused;
{
#if !__rtems__
	/*
	 * When doing reverse DNS lookups, the finish_up flag might not
	 * be noticed for a while.  Just exit if we get a second SIGINT.
	 */
	if (!(options_ & F_NUMERIC) && finish_up)
		_exit(nreceived ? 0 : 2);
	finish_up = 1;
#endif
}

/*
 * pinger --
 *	Compose and transmit an ICMP ECHO REQUEST packet.  The IP packet
 * will be added on by the kernel.  The ID field is our UNIX process ID,
 * and the sequence number is an ascending integer.  The first TIMEVAL_LEN
 * bytes of the data portion are used to hold a UNIX "timeval" struct in
 * host byte-order, to compute the round-trip time.
 */
static void
g_pinger(globals)
  rtems_shell_globals_t* globals;
{
	struct timeval now;
	struct tv32 tv32;
	struct ip *ip;
	struct icmp *icp;
	int cc, i;
	u_char *packet;

	packet = outpack;
	icp = (struct icmp *)outpack;
	icp->icmp_type = icmp_type_;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = htons(ntransmitted);
	icp->icmp_id = ident;			/* ID */

	if(mx_dup_ck == 0){
		mx_dup_ck = MAX_DUP_CHK;
	}
	CLR(ntransmitted % mx_dup_ck);

	if ((options_ & F_TIME) || timing) {
		(void)gettimeofday(&now, NULL);

		tv32.tv32_sec = htonl(now.tv_sec);
		tv32.tv32_usec = htonl(now.tv_usec);
		if (options_ & F_TIME)
			icp->icmp_otime = htonl((now.tv_sec % (24*60*60))
				* 1000 + now.tv_usec / 1000);
		if (timing)
			bcopy((void *)&tv32,
			    (void *)&outpack[ICMP_MINLEN + phdr_len],
			    sizeof(tv32));
	}

	cc = ICMP_MINLEN + phdr_len + datalen;

	/* compute ICMP checksum here */
	icp->icmp_cksum = in_cksum((u_short *)icp, cc);

	if (options_ & F_HDRINCL) {
		cc += sizeof(struct ip);
		ip = (struct ip *)outpackhdr;
		ip->ip_len = cc;
		ip->ip_sum = in_cksum((u_short *)outpackhdr, cc);
		packet = outpackhdr;
	}
	i = sendto(s, (char *)packet, cc, 0, (struct sockaddr *)&whereto,
	    sizeof(whereto));

	if (i < 0 || i != cc)  {
		if (i < 0) {
			if (options_ & F_FLOOD && errno == ENOBUFS) {
				usleep(FLOOD_BACKOFF);
				return;
			}
			warn("sendto");
		} else {
			warn("%s: partial write: %d of %d bytes",
			     hostname, i, cc);
		}
	}
	ntransmitted++;
	sntransmitted++;
	if (!(options_ & F_QUIET) && options_ & F_FLOOD)
		(void)write(STDOUT_FILENO, &DOT, 1);
}

/*
 * pr_pack --
 *	Print out the packet, if it came from us.  This logic is necessary
 * because ALL readers of the ICMP socket get a copy of ALL ICMP packets
 * which arrive ('tis only fair).  This permits multiple copies of this
 * program to be run without having intermingled output (or statistics!).
 */
static void
g_pr_pack(buf, cc, from, tv, globals)
	char *buf;
	int cc;
	struct sockaddr_in *from;
	struct timeval *tv;
  rtems_shell_globals_t* globals;
{
	struct in_addr ina;
	u_char *cp, *dp;
	struct icmp *icp;
	struct ip *ip;
	const void *tp;
	double triptime;
	int dupflag, hlen, i, j, recv_len, seq;
#if !__rtems__
	static int old_rrlen;
	static char old_rr[MAX_IPOPTLEN];
#endif

	/* Check the IP header */
	ip = (struct ip *)buf;
	hlen = ip->ip_hl << 2;
	recv_len = cc;
	if (cc < hlen + ICMP_MINLEN) {
		if (options_ & F_VERBOSE)
			warn("packet too short (%d bytes) from %s", cc,
			     inet_ntoa(from->sin_addr));
		return;
	}

	/* Now the ICMP part */
	cc -= hlen;
	icp = (struct icmp *)(buf + hlen);
	if (icp->icmp_type == icmp_type_rsp) {
		if (icp->icmp_id != ident)
			return;			/* 'Twas not our ECHO */
		++nreceived;
		triptime = 0.0;
		if (timing) {
			struct timeval tv1;
			struct tv32 tv32;
#ifndef icmp_data
			tp = &icp->icmp_ip;
#else
			tp = icp->icmp_data;
#endif
			tp = (const char *)tp + phdr_len;

			if (cc - ICMP_MINLEN - phdr_len >= (int) sizeof(tv1)) {
				/* Copy to avoid alignment problems: */
				memcpy(&tv32, tp, sizeof(tv32));
				tv1.tv_sec = ntohl(tv32.tv32_sec);
				tv1.tv_usec = ntohl(tv32.tv32_usec);
				tvsub(tv, &tv1);
 				triptime = ((double)tv->tv_sec) * 1000.0 +
 				    ((double)tv->tv_usec) / 1000.0;
				tsum += triptime;
				tsumsq += triptime * triptime;
				if (triptime < tmin)
					tmin = triptime;
				if (triptime > tmax)
					tmax = triptime;
			} else
				timing = 0;
		}

		seq = ntohs(icp->icmp_seq);

		if(mx_dup_ck == 0){
			mx_dup_ck = MAX_DUP_CHK;
		}

		if (TST(seq % mx_dup_ck	)) {
			++nrepeats;
			--nreceived;
			dupflag = 1;
		} else {
			SET(seq % mx_dup_ck	);
			dupflag = 0;
		}

		if (options_ & F_QUIET)
			return;

		if (options_ & F_WAITTIME && triptime > waittime) {
			++nrcvtimeout;
			return;
		}

		if (options_ & F_FLOOD)
			(void)write(STDOUT_FILENO, &BSPACE, 1);
		else {
			(void)printf("%d bytes from %s: icmp_seq=%u", cc,
			   inet_ntoa(*(struct in_addr *)&from->sin_addr.s_addr),
			   seq);
			(void)printf(" ttl=%d", ip->ip_ttl);
			if (timing)
				(void)printf(" time=%.3f ms", triptime);
			if (dupflag)
				(void)printf(" (DUP!)");
			if (options_ & F_AUDIBLE)
				(void)write(STDOUT_FILENO, &BBELL, 1);
			if (options_ & F_MASK) {
				/* Just prentend this cast isn't ugly */
				(void)printf(" mask=%s",
					pr_addr(*(struct in_addr *)&(icp->icmp_mask)));
			}
			if (options_ & F_TIME) {
				(void)printf(" tso=%s", pr_ntime(icp->icmp_otime));
				(void)printf(" tsr=%s", pr_ntime(icp->icmp_rtime));
				(void)printf(" tst=%s", pr_ntime(icp->icmp_ttime));
			}
			if (recv_len != send_len) {
                        	(void)printf(
				     "\nwrong total length %d instead of %d",
				     recv_len, send_len);
			}
			/* check the data */
			cp = (u_char*)&icp->icmp_data[phdr_len];
			dp = &outpack[ICMP_MINLEN + phdr_len];
			cc -= ICMP_MINLEN + phdr_len;
			i = 0;
			if (timing) {   /* don't check variable timestamp */
				cp += TIMEVAL_LEN;
				dp += TIMEVAL_LEN;
				cc -= TIMEVAL_LEN;
				i += TIMEVAL_LEN;
			}
			for (; i < datalen && cc > 0; ++i, ++cp, ++dp, --cc) {
				if (*cp != *dp) {
	(void)printf("\nwrong data byte #%d should be 0x%x but was 0x%x",
	    i, *dp, *cp);
					(void)printf("\ncp:");
					cp = (u_char*)&icp->icmp_data[0];
					for (i = 0; i < datalen; ++i, ++cp) {
						if ((i % 16) == 8)
							(void)printf("\n\t");
						(void)printf("%2x ", *cp);
					}
					(void)printf("\ndp:");
					cp = &outpack[ICMP_MINLEN];
					for (i = 0; i < datalen; ++i, ++cp) {
						if ((i % 16) == 8)
							(void)printf("\n\t");
						(void)printf("%2x ", *cp);
					}
					break;
				}
			}
		}
	} else {
		/*
		 * We've got something other than an ECHOREPLY.
		 * See if it's a reply to something that we sent.
		 * We can compare IP destination, protocol,
		 * and ICMP type and ID.
		 *
		 * Only print all the error messages if we are running
		 * as root to avoid leaking information not normally
		 * available to those not running as root.
		 */
#ifndef icmp_data
		struct ip *oip = &icp->icmp_ip;
#else
		struct ip *oip = (struct ip *)icp->icmp_data;
#endif
		struct icmp *oicmp = (struct icmp *)(oip + 1);

		if (((options_ & F_VERBOSE) && uid == 0) ||
		    (!(options_ & F_QUIET2) &&
		     (oip->ip_dst.s_addr == whereto.sin_addr.s_addr) &&
		     (oip->ip_p == IPPROTO_ICMP) &&
		     (oicmp->icmp_type == ICMP_ECHO) &&
		     (oicmp->icmp_id == ident))) {
		    (void)printf("%d bytes from %s: ", cc,
			pr_addr(from->sin_addr));
		    pr_icmph(icp);
		} else
		    return;
	}

	/* Display any IP options */
	cp = (u_char *)buf + sizeof(struct ip);

	for (; hlen > (int)sizeof(struct ip); --hlen, ++cp)
		switch (*cp) {
		case IPOPT_EOL:
			hlen = 0;
			break;
		case IPOPT_LSRR:
		case IPOPT_SSRR:
			(void)printf(*cp == IPOPT_LSRR ?
			    "\nLSRR: " : "\nSSRR: ");
			j = cp[IPOPT_OLEN] - IPOPT_MINOFF + 1;
			hlen -= 2;
			cp += 2;
			if (j >= INADDR_LEN &&
			    j <= hlen - (int)sizeof(struct ip)) {
				for (;;) {
					bcopy(++cp, &ina.s_addr, INADDR_LEN);
					if (ina.s_addr == 0)
						(void)printf("\t0.0.0.0");
					else
						(void)printf("\t%s",
						     pr_addr(ina));
					hlen -= INADDR_LEN;
					cp += INADDR_LEN - 1;
					j -= INADDR_LEN;
					if (j < INADDR_LEN)
						break;
					(void)putchar('\n');
				}
			} else
				(void)printf("\t(truncated route)\n");
			break;
		case IPOPT_RR:
			j = cp[IPOPT_OLEN];		/* get length */
			i = cp[IPOPT_OFFSET];		/* and pointer */
			hlen -= 2;
			cp += 2;
			if (i > j)
				i = j;
			i = i - IPOPT_MINOFF + 1;
			if (i < 0 || i > (hlen - (int)sizeof(struct ip))) {
				old_rrlen = 0;
				continue;
			}
			if (i == old_rrlen
			    && !bcmp((char *)cp, old_rr, i)
			    && !(options_ & F_FLOOD)) {
				(void)printf("\t(same route)");
				hlen -= i;
				cp += i;
				break;
			}
			old_rrlen = i;
			bcopy((char *)cp, old_rr, i);
			(void)printf("\nRR: ");
			if (i >= INADDR_LEN &&
			    i <= hlen - (int)sizeof(struct ip)) {
				for (;;) {
					bcopy(++cp, &ina.s_addr, INADDR_LEN);
					if (ina.s_addr == 0)
						(void)printf("\t0.0.0.0");
					else
						(void)printf("\t%s",
						     pr_addr(ina));
					hlen -= INADDR_LEN;
					cp += INADDR_LEN - 1;
					i -= INADDR_LEN;
					if (i < INADDR_LEN)
						break;
					(void)putchar('\n');
				}
			} else
				(void)printf("\t(truncated route)");
			break;
		case IPOPT_NOP:
			(void)printf("\nNOP");
			break;
		default:
			(void)printf("\nunknown option %x", *cp);
			break;
		}
	if (!(options_ & F_FLOOD)) {
		(void)putchar('\n');
		(void)fflush(stdout);
	}
}

/*
 * in_cksum --
 *	Checksum routine for Internet Protocol family headers (C Version)
 */
u_short
in_cksum(addr, len)
	u_short *addr;
	int len;
{
	int nleft, sum;
	u_short *w;
	union {
		u_short	us;
		u_char	uc[2];
	} last;
	u_short answer;

	nleft = len;
	sum = 0;
	w = addr;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		last.uc[0] = *(u_char *)w;
		last.uc[1] = 0;
		sum += last.us;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
}

/*
 * tvsub --
 *	Subtract 2 timeval structs:  out = out - in.  Out is assumed to
 * be >= in.
 */
static void
tvsub(out, in)
	struct timeval *out, *in;
{

	if ((out->tv_usec -= in->tv_usec) < 0) {
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

/*
 * status --
 *	Print out statistics when SIGINFO is received.
 */

#if !defined(__rtems__)
static void
status(sig)
	int sig __unused;
{
	siginfo_p = 1;
}
#endif

static void
g_check_status(globals)
  rtems_shell_globals_t* globals;
{
	if (siginfo_p) {
		siginfo_p = 0;
		(void)fprintf(stderr, "\r%ld/%ld packets received (%.1f%%)",
		    nreceived, ntransmitted,
		    ntransmitted ? nreceived * 100.0 / ntransmitted : 0.0);
		if (nreceived && timing)
			(void)fprintf(stderr, " %.3f min / %.3f avg / %.3f max",
			    tmin, tsum / (nreceived + nrepeats), tmax);
		(void)fprintf(stderr, "\n");
	}
}

/*
 * finish --
 *	Print out statistics, and give up.
 */
static void
g_finish(globals)
  rtems_shell_globals_t* globals;
{

	(void)signal(SIGINT, SIG_IGN);
	(void)signal(SIGALRM, SIG_IGN);
	(void)putchar('\n');
	(void)fflush(stdout);
	(void)printf("--- %s ping statistics ---\n", hostname);
	(void)printf("%ld packets transmitted, ", ntransmitted);
	(void)printf("%ld packets received, ", nreceived);
	if (nrepeats)
		(void)printf("+%ld duplicates, ", nrepeats);
	if (ntransmitted) {
		if (nreceived > ntransmitted)
			(void)printf("-- somebody's printing up packets!");
		else
			(void)printf("%.1f%% packet loss",
			    ((ntransmitted - nreceived) * 100.0) /
			    ntransmitted);
	}
	if (nrcvtimeout)
		(void)printf(", %ld packets out of wait time", nrcvtimeout);
	(void)putchar('\n');
	if (nreceived && timing) {
		double n = nreceived + nrepeats;
		double avg = tsum / n;
#if defined(__rtems__)
		(void) printf(
		    "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f ms\n",
		    tmin, avg, tmax);
#else
		double vari = tsumsq / n - avg * avg;
		(void)printf(
		    "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		    tmin, avg, tmax, sqrt(vari));
#endif
	}
	if (nreceived)
		exit(0);
	else
		exit(2);
  while (1);
}

#ifdef notdef
static char *ttab[] = {
	"Echo Reply",		/* ip + seq + udata */
	"Dest Unreachable",	/* net, host, proto, port, frag, sr + IP */
	"Source Quench",	/* IP */
	"Redirect",		/* redirect type, gateway, + IP  */
	"Echo",
	"Time Exceeded",	/* transit, frag reassem + IP */
	"Parameter Problem",	/* pointer + IP */
	"Timestamp",		/* id + seq + three timestamps */
	"Timestamp Reply",	/* " */
	"Info Request",		/* id + sq */
	"Info Reply"		/* " */
};
#endif

/*
 * pr_icmph --
 *	Print a descriptive string about an ICMP header.
 */
static void
pr_icmph(icp)
	struct icmp *icp;
{

	switch(icp->icmp_type) {
	case ICMP_ECHOREPLY:
		(void)printf("Echo Reply\n");
		/* XXX ID + Seq + Data */
		break;
	case ICMP_UNREACH:
		switch(icp->icmp_code) {
		case ICMP_UNREACH_NET:
			(void)printf("Destination Net Unreachable\n");
			break;
		case ICMP_UNREACH_HOST:
			(void)printf("Destination Host Unreachable\n");
			break;
		case ICMP_UNREACH_PROTOCOL:
			(void)printf("Destination Protocol Unreachable\n");
			break;
		case ICMP_UNREACH_PORT:
			(void)printf("Destination Port Unreachable\n");
			break;
		case ICMP_UNREACH_NEEDFRAG:
			(void)printf("frag needed and DF set (MTU %d)\n",
					ntohs(icp->icmp_nextmtu));
			break;
		case ICMP_UNREACH_SRCFAIL:
			(void)printf("Source Route Failed\n");
			break;
		case ICMP_UNREACH_FILTER_PROHIB:
			(void)printf("Communication prohibited by filter\n");
			break;
		default:
			(void)printf("Dest Unreachable, Bad Code: %d\n",
			    icp->icmp_code);
			break;
		}
		/* Print returned IP header information */
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_SOURCEQUENCH:
		(void)printf("Source Quench\n");
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_REDIRECT:
		switch(icp->icmp_code) {
		case ICMP_REDIRECT_NET:
			(void)printf("Redirect Network");
			break;
		case ICMP_REDIRECT_HOST:
			(void)printf("Redirect Host");
			break;
		case ICMP_REDIRECT_TOSNET:
			(void)printf("Redirect Type of Service and Network");
			break;
		case ICMP_REDIRECT_TOSHOST:
			(void)printf("Redirect Type of Service and Host");
			break;
		default:
			(void)printf("Redirect, Bad Code: %d", icp->icmp_code);
			break;
		}
		(void)printf("(New addr: %s)\n", inet_ntoa(icp->icmp_gwaddr));
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_ECHO:
		(void)printf("Echo Request\n");
		/* XXX ID + Seq + Data */
		break;
	case ICMP_TIMXCEED:
		switch(icp->icmp_code) {
		case ICMP_TIMXCEED_INTRANS:
			(void)printf("Time to live exceeded\n");
			break;
		case ICMP_TIMXCEED_REASS:
			(void)printf("Frag reassembly time exceeded\n");
			break;
		default:
			(void)printf("Time exceeded, Bad Code: %d\n",
			    icp->icmp_code);
			break;
		}
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_PARAMPROB:
		(void)printf("Parameter problem: pointer = 0x%02x\n",
		    icp->icmp_hun.ih_pptr);
#ifndef icmp_data
		pr_retip(&icp->icmp_ip);
#else
		pr_retip((struct ip *)icp->icmp_data);
#endif
		break;
	case ICMP_TSTAMP:
		(void)printf("Timestamp\n");
		/* XXX ID + Seq + 3 timestamps */
		break;
	case ICMP_TSTAMPREPLY:
		(void)printf("Timestamp Reply\n");
		/* XXX ID + Seq + 3 timestamps */
		break;
	case ICMP_IREQ:
		(void)printf("Information Request\n");
		/* XXX ID + Seq */
		break;
	case ICMP_IREQREPLY:
		(void)printf("Information Reply\n");
		/* XXX ID + Seq */
		break;
	case ICMP_MASKREQ:
		(void)printf("Address Mask Request\n");
		break;
	case ICMP_MASKREPLY:
		(void)printf("Address Mask Reply\n");
		break;
	case ICMP_ROUTERADVERT:
		(void)printf("Router Advertisement\n");
		break;
	case ICMP_ROUTERSOLICIT:
		(void)printf("Router Solicitation\n");
		break;
	default:
		(void)printf("Bad ICMP type: %d\n", icp->icmp_type);
	}
}

/*
 * pr_iph --
 *	Print an IP header with options.
 */
static void
pr_iph(ip)
	struct ip *ip;
{
	u_char *cp;
	int hlen;

	hlen = ip->ip_hl << 2;
	cp = (u_char *)ip + 20;		/* point to options */

	(void)printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst\n");
	(void)printf(" %1x  %1x  %02x %04x %04x",
	    ip->ip_v, ip->ip_hl, ip->ip_tos, ntohs(ip->ip_len),
	    ntohs(ip->ip_id));
	(void)printf("   %1lx %04lx",
	    (u_long) (ntohl(ip->ip_off) & 0xe000) >> 13,
	    (u_long) ntohl(ip->ip_off) & 0x1fff);
	(void)printf("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p,
							    ntohs(ip->ip_sum));
	(void)printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_src.s_addr));
	(void)printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->ip_dst.s_addr));
	/* dump any option bytes */
	while (hlen-- > 20) {
		(void)printf("%02x", *cp++);
	}
	(void)putchar('\n');
}

/*
 * pr_addr --
 *	Return an ascii host address as a dotted quad and optionally with
 * a hostname.
 */
static char *
g_pr_addr(ina, globals)
	struct in_addr ina;
  rtems_shell_globals_t* globals;
{
	struct hostent *hp;
	static char buf[16 + 3 + MAXHOSTNAMELEN];

	if ((options_ & F_NUMERIC) ||
	    !(hp = gethostbyaddr((char *)&ina, 4, AF_INET)))
		return inet_ntoa(ina);
	else
		(void)snprintf(buf, sizeof(buf), "%s (%s)", hp->h_name,
		    inet_ntoa(ina));
	return(buf);
}

/*
 * pr_retip --
 *	Dump some info on a returned (via ICMP) IP packet.
 */
static void
pr_retip(ip)
	struct ip *ip;
{
	u_char *cp;
	int hlen;

	pr_iph(ip);
	hlen = ip->ip_hl << 2;
	cp = (u_char *)ip + hlen;

	if (ip->ip_p == 6)
		(void)printf("TCP: from port %u, to port %u (decimal)\n",
		    (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
	else if (ip->ip_p == 17)
		(void)printf("UDP: from port %u, to port %u (decimal)\n",
			(*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
}

static char *
pr_ntime (n_time timestamp)
{
	static char buf[10];
	int hour, min, sec;

	sec = ntohl(timestamp) / 1000;
	hour = sec / 60 / 60;
	min = (sec % (60 * 60)) / 60;
	sec = (sec % (60 * 60)) % 60;

	(void)snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, min, sec);

	return (buf);
}

static void
g_fill(bp, patp, globals)
	char *bp, *patp;
  rtems_shell_globals_t* globals;
{
	char *cp;
	int pat[16];
	u_int ii, jj, kk;

	for (cp = patp; *cp; cp++) {
		if (!isxdigit((int)*cp))
			errx(&globals->exit_jmp, EX_USAGE,
			    "patterns must be specified as hex digits");

	}
	ii = sscanf(patp,
	    "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	    &pat[0], &pat[1], &pat[2], &pat[3], &pat[4], &pat[5], &pat[6],
	    &pat[7], &pat[8], &pat[9], &pat[10], &pat[11], &pat[12],
	    &pat[13], &pat[14], &pat[15]);

	if (ii > 0)
		for (kk = 0; kk <= maxpayload - (TIMEVAL_LEN + ii); kk += ii)
			for (jj = 0; jj < ii; ++jj)
				bp[jj + kk] = pat[jj];
	if (!(options_ & F_QUIET)) {
		(void)printf("PATTERN: 0x");
		for (jj = 0; jj < ii; ++jj)
			(void)printf("%02x", bp[jj] & 0xFF);
		(void)printf("\n");
	}
}

#if defined(IPSEC) && defined(IPSEC_POLICY_IPSEC)
#define	SECOPT		" [-P policy]"
#else
#define	SECOPT		""
#endif
static void
g_usage(globals)
  rtems_shell_globals_t* globals;
{
	(void)fprintf(stderr, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
"usage: ping [-AaDdfnoQqRrv] [-c count] [-G sweepmaxsize] [-g sweepminsize]",
"            [-h sweepincrsize] [-i wait] [-l preload] [-M mask | time] [-m ttl]",
"           " SECOPT " [-p pattern] [-S src_addr] [-s packetsize] [-t timeout]",
"            [-W waittime] [-z tos] host",
"       ping [-AaDdfLnoQqRrv] [-c count] [-I iface] [-i wait] [-l preload]",
"            [-M mask | time] [-m ttl]" SECOPT " [-p pattern] [-S src_addr]",
"            [-s packetsize] [-T ttl] [-t timeout] [-W waittime]",
"            [-z tos] mcast-group");
	exit(EX_USAGE);
  while (1);
}

#if __rtems__
  rtems_shell_cmd_t rtems_shell_PING_Command = {
    "ping",                        /* name */
    "ping [args]",                 /* usage */
    "network",                     /* topic */
    rtems_shell_main_ping,         /* command */
    NULL,                          /* alias */
    NULL                           /* next */
  };
#endif
