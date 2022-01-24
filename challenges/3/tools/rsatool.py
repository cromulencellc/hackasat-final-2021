#!/usr/bin/env python3

#####
# Source: https://github.com/ius/rsatool
# 
# Edited to include asn for multiprime RSA as well as a few other small edits
#####

import base64
import math
import random
import gmpy

from pyasn1.codec.der import encoder
from pyasn1.type.univ import Sequence, Integer

PEM_TEMPLATE = b'-----BEGIN RSA PRIVATE KEY-----\n%s-----END RSA PRIVATE KEY-----\n'
DEFAULT_EXP = 2**16+1

# https://github.com/ius/rsatool
def factor_modulus(n, d, e):
    """
    Efficiently recover non-trivial factors of n

    See: Handbook of Applied Cryptography
    8.2.2 Security of RSA -> (i) Relation to factoring (p.287)

    http://www.cacr.math.uwaterloo.ca/hac/
    """
    t = (e * d - 1)
    s = 0

    while True:
        quotient, remainder = divmod(t, 2)

        if remainder != 0:
            break

        s += 1
        t = quotient

    p, q, r = 1, 1, 1
    while (p * q * r) != n:
        p, q, r = 1, 1, 1
        found_0, found_1, found_2 = False, False, False

        while not found_0:
            i = 1
            a = random.randint(1, n-1)

            while i <= s and not found_0:
                num0_c1 = pow(a, pow(2, i-1, n) * t, n)
                num0_c2 = pow(a, pow(2, i, n) * t, n)

                found_0 = num0_c1 != 1 and num0_c1 != (-1 % n) and num0_c2 == 1

                i += 1

        while not found_1:
            i = 1
            a = random.randint(1, n-1)

            while i <= s and not found_1:
                num1_c1 = pow(a, pow(2, i-1, n) * t, n)
                num1_c2 = pow(a, pow(2, i, n) * t, n)

                found_1 = num1_c1 != 1 and num1_c1 != (-1 % n) and num1_c2 == 1

                i += 1
                if num1_c1 == num0_c1:
                    found_1 = False
        while not found_2:
            i = 1
            a = random.randint(1, n-1)

            while i <= s and not found_2:
                num2_c1 = pow(a, pow(2, i-1, n) * t, n)
                num2_c2 = pow(a, pow(2, i, n) * t, n)

                found_2 = num2_c1 != 1 and num2_c1 != (-1 % n) and num2_c2 == 1

                i += 1
                if num2_c1 == num1_c1 or num2_c1 == num0_c1:
                    found_2 = False
                    
        n_0 = math.gcd(num0_c1-1, n)
        n_1 = math.gcd(num1_c1-1, n)
        n_2 = math.gcd(num2_c1-1, n)
        gcd_nums = [n_0, n_1, n_2]
        
        x = math.gcd(n_0, n_1)
        y = math.gcd(n_1, n_2)
        z = math.gcd(n_0, n_2)
        r = max(x,y,z)
        
        rem = n // r
        x = math.gcd(rem, n_0)
        y = math.gcd(rem, n_1)
        z = math.gcd(rem, n_2)
        q = max(x,y,z)
        
        p = n // r // q
        
        o = [p,q,r]
        o.sort()
        
        p,q,r = o[1], o[0], o[2]
        
        if not (p!=1 and q!=1 and r!=1) or not (gmpy.is_prime(p) and gmpy.is_prime(q) and gmpy.is_prime(r)):
            p,q,r=1,1,1
            continue

    return p, q, r

class RSATool:
    def __init__(self, n=None, d=None, e=DEFAULT_EXP):
        """
        Initialize RSA instance using primes (p, q)
        or modulus and private exponent (n, d)
        """

        self.e = e
        self.p, self.q, self.r = factor_modulus(n, d, e)

        self._calc_values()

    def _calc_values(self):
        self.n = self.p * self.q * self.r

        if self.p != self.q:
            phi = (self.p - 1) * (self.q - 1) * (self.r - 1)
        else:
            phi = (self.p ** 2) - self.p

        self.d = gmpy.invert(self.e, phi)

        # CRT-RSA precomputation
        self.dP = self.d % (self.p - 1)
        self.dQ = self.d % (self.q - 1)
        self.qInv = gmpy.invert(self.q, self.p)
        self.pInv = gmpy.invert(self.p, self.q)

        # Other prime info
        self.dR = self.d % (self.r - 1)
        self.rInv = gmpy.invert(self.p * self.q, self.r)

    def to_pem(self):
        """
        Return OpenSSL-compatible PEM encoded key
        """
        key = base64.encodebytes(self.to_der())
        key = key.replace(b"\n", b"")
        
        key_nl = b""
        for idx in range(0, len(key), 64):
            key_nl += key[idx:idx+64] + b"\n"
        return PEM_TEMPLATE % key_nl

    # https://datatracker.ietf.org/doc/html/rfc3447 -- Page 44
    # ASN definition for RSA (including multiprime)
    def to_der(self):
        """
        Return parameters as OpenSSL compatible DER encoded key
        """
        seq = Sequence()
        seq_info_primes = Sequence()
        seq_other_prime = Sequence()
        
        for idx, x in enumerate([self.r, self.dR, self.rInv]):
            seq_info_primes.setComponentByPosition(idx, Integer(x))
        
        seq_other_prime.setComponentByPosition(0, seq_info_primes)
        
        for idx, x in enumerate([1, self.n, self.e, self.d, self.p, self.q, self.dP, self.dQ, self.qInv]):
            seq.setComponentByPosition(idx, Integer(x))
        
        seq.setComponentByPosition(9, seq_other_prime)
        
        return encoder.encode(seq)

    def dump(self, verbose):
        vars = ['n', 'e', 'd', 'p', 'q']

        if verbose:
            vars += ['dP', 'dQ', 'qInv']

        for v in vars:
            self._dumpvar(v)

    def _dumpvar(self, var):
        val = getattr(self, var)

        def parts(s, l): return '\n'.join(
            [s[i:i+l] for i in range(0, len(s), l)])

        if len(str(val)) <= 40:
            print('%s = %d (%#x)\n' % (var, val, val))
        else:
            print('%s =' % var)
            print(parts('%x' % val, 80) + '\n')
