#!/usr/bin/env python3
"""
Simple UDP probe for GenX320 DVS Ethernet streaming.

Receives raw EVT2.0 uint32 words and prints live throughput statistics.
"""

import argparse
import socket
import struct
import time


def parse_args():
    parser = argparse.ArgumentParser(description="Probe DVS UDP stream")
    parser.add_argument("--bind", default="0.0.0.0", help="Local bind IP (default: 0.0.0.0)")
    parser.add_argument("--port", type=int, default=7777, help="UDP listen port (default: 7777)")
    parser.add_argument("--duration", type=float, default=0.0, help="Run time in seconds (0 = forever)")
    parser.add_argument("--timeout", type=float, default=1.0, help="Socket timeout in seconds")
    return parser.parse_args()


def main():
    args = parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.bind, args.port))
    sock.settimeout(args.timeout)

    print(f"Listening on {args.bind}:{args.port}")

    t0 = time.time()
    last = t0
    pkt_total = 0
    byte_total = 0
    word_total = 0
    nonzero_total = 0
    last_src = None

    while True:
        now = time.time()
        if args.duration > 0 and (now - t0) >= args.duration:
            break

        try:
            data, src = sock.recvfrom(2048)
        except socket.timeout:
            data = None
            src = None

        now = time.time()

        if data:
            last_src = src
            pkt_total += 1
            byte_total += len(data)

            words = len(data) // 4
            word_total += words
            if words:
                fmt = "<" + ("I" * words)
                unpacked = struct.unpack(fmt, data[: words * 4])
                nonzero_total += sum(1 for w in unpacked if w != 0)

        if (now - last) >= 1.0:
            dt = now - last
            pps = pkt_total / max(now - t0, 1e-9)
            mbps = (byte_total * 8.0) / max(now - t0, 1e-9) / 1e6
            wps = word_total / max(now - t0, 1e-9)
            nz = (100.0 * nonzero_total / word_total) if word_total else 0.0

            src_str = f"{last_src[0]}:{last_src[1]}" if last_src else "none"
            print(
                f"src={src_str} pkts={pkt_total} bytes={byte_total} "
                f"avg_pps={pps:8.1f} avg_mbps={mbps:6.2f} avg_words/s={wps:9.0f} nonzero={nz:5.1f}%"
            )
            last = now

    print("Done.")


if __name__ == "__main__":
    main()
