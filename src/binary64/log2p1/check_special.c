/* Check log2p1 on random inputs.

Copyright (c) 2022-2023 Paul Zimmermann, Inria.

This file is part of the CORE-MATH project
(https://core-math.gitlabpages.inria.fr/).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <omp.h>

int ref_init (void);
int ref_fesetround (int);

double cr_log2p1 (double);
double ref_log2p1 (double);

int rnd1[] = { FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD };

int rnd = 0;
int verbose = 0;

static inline uint64_t
asuint64 (double f)
{
  union
  {
    double f;
    uint64_t i;
  } u = {f};
  return u.i;
}

typedef union {double f; uint64_t u;} b64u64_u;

static double
get_random ()
{
  b64u64_u v;
  long l;
  v.u = rand ();
  v.u |= (uint64_t) rand () << 31;
  v.u |= (uint64_t) rand () << 62;
  return v.f;
}

static void
check (double x)
{
  int bug;
  double y1 = ref_log2p1 (x);
  fesetround (rnd1[rnd]);
  double y2 = cr_log2p1 (x);
  if (isnan (y1))
    bug = !isnan (y2);
  else if (isnan (y2))
    bug = !isnan (y1);
  else
    bug = asuint64 (y1) != asuint64 (y2);
  if (bug)
  {
    printf ("FAIL x=%la ref=%la z=%la\n", x, y1, y2);
    fflush (stdout);
    exit (1);
  }
}

#define N_WORST 1024

/* worst cases in [2^-1022,2^-1021) */
static double T[N_WORST] = {
0x1.007cb79fb8dbp-1022,
0x1.01491f3c1498bp-1022,
0x1.00a390b41edd6p-1022,
0x1.016ff8507a9b1p-1022,
0x1.00ca69c884dfcp-1022,
0x1.01224627ae965p-1022,
0x1.01eeadc40a54p-1022,
0x1.02bb15606611bp-1022,
0x1.021586d870566p-1022,
0x1.02e1ee74cc141p-1022,
0x1.023c5fecd658cp-1022,
0x1.02943c4c000f5p-1022,
0x1.03877cfcc1cf6p-1022,
0x1.0453e4991d8d1p-1022,
0x1.0406327051885p-1022,
0x1.0360a3e85bcdp-1022,
0x1.042d0b84b78abp-1022,
0x1.04f9732113486p-1022,
0x1.05c5dabd6f061p-1022,
0x1.04abc0f84743ap-1022,
0x1.061db71c98bcap-1022,
0x1.05782894a3015p-1022,
0x1.04d29a0cad46p-1022,
0x1.059f01a90903bp-1022,
0x1.06ea1eb8f47a5p-1022,
0x1.07b686555038p-1022,
0x1.0737d0e1c07f1p-1022,
0x1.066b694564c16p-1022,
0x1.078fad40ea35ap-1022,
0x1.06449030febfp-1022,
0x1.0710f7cd5a7cbp-1022,
0x1.07dd5f69b63a6p-1022,
0x1.0901a3653baeap-1022,
0x1.085c14dd45f35p-1022,
0x1.09ce0b01976c5p-1022,
0x1.09287c79a1b1p-1022,
0x1.0882edf1abf5bp-1022,
0x1.094f558e07b36p-1022,
0x1.0a9a729df32ap-1022,
0x1.09f4e415fd6ebp-1022,
0x1.0ac14bb2592c6p-1022,
0x1.0a7399898d27ap-1022,
0x1.0b66da3a4ee7bp-1022,
0x1.0b19281182e2fp-1022,
0x1.0c0c68c244a3p-1022,
0x1.0b400125e8e55p-1022,
0x1.0cd8d05ea060bp-1022,
0x1.0c3341d6aaa56p-1022,
0x1.0be58faddea0ap-1022,
0x1.0c8b1e35d45bfp-1022,
0x1.0cb1f74a3a5e5p-1022,
0x1.0da537fafc1e6p-1022,
0x1.0dfd145a25d4fp-1022,
0x1.0d5785d23019ap-1022,
0x1.0d7e5ee6961cp-1022,
0x1.0e23ed6e8bd75p-1022,
0x1.0e4ac682f1d9bp-1022,
0x1.0f6f0a7e774dfp-1022,
0x1.0ec97bf68192ap-1022,
0x1.0ef0550ae795p-1022,
0x1.0f95e392dd505p-1022,
0x1.101499066d094p-1022,
0x1.0fbcbca74352bp-1022,
0x1.10e100a2c8c6fp-1022,
0x1.103b721ad30bap-1022,
0x1.1107d9b72ec95p-1022,
0x1.10624b2f390ep-1022,
0x1.112eb2cb94cbbp-1022,
0x1.11868f2abe824p-1022,
0x1.11ad683f2484ap-1022,
0x1.1279cfdb80425p-1022,
0x1.11d441538a87p-1022,
0x1.131f5e6375fdap-1022,
0x1.12a0a8efe644bp-1022,
0x1.1252f6c71a3ffp-1022,
0x1.12f8854f0ffb4p-1022,
0x1.13ebc5ffd1bb5p-1022,
0x1.13463777dcp-1022,
0x1.13c4eceb6bb8fp-1022,
0x1.146a7b7361744p-1022,
0x1.14915487c776ap-1022,
0x1.14b82d9c2d79p-1022,
0x1.162a23c07ef2p-1022,
0x1.151009fb572f9p-1022,
0x1.1536e30fbd31fp-1022,
0x1.15dc7197b2ed4p-1022,
0x1.155dbc2423345p-1022,
0x1.16034aac18efap-1022,
0x1.1682001fa8a89p-1022,
0x1.174e67bc04664p-1022,
0x1.16a8d9340eaafp-1022,
0x1.16cfb24874ad5p-1022,
0x1.177540d06a68ap-1022,
0x1.179c19e4d06bp-1022,
0x1.17f3f643fa219p-1022,
0x1.18c05de055df4p-1022,
0x1.181acf586023fp-1022,
0x1.1841a86cc6265p-1022,
0x1.18e736f4bbe1ap-1022,
0x1.190e100921e4p-1022,
0x1.1a325404a7584p-1022,
0x1.198cc57cb19cfp-1022,
0x1.1a592d190d5aap-1022,
0x1.1965ec684b9a9p-1022,
0x1.19b39e91179f5p-1022,
0x1.1afebba10315fp-1022,
0x1.1ad7e28c9d139p-1022,
0x1.1bcb233d5ed3ap-1022,
0x1.1b2594b569185p-1022,
0x1.1b7d711492ceep-1022,
0x1.1ba44a28f8d14p-1022,
0x1.1c49d8b0ee8c9p-1022,
0x1.1d3d1961b04cap-1022,
0x1.1c978ad9ba915p-1022,
0x1.1c70b1c5548efp-1022,
0x1.1cef6738e447ep-1022,
0x1.1d16404d4a4a4p-1022,
0x1.1dbbced540059p-1022,
0x1.1e0980fe0c0a5p-1022,
0x1.1de2a7e9a607fp-1022,
0x1.1e615d5d35c0ep-1022,
0x1.1e8836719bc34p-1022,
0x1.1f2dc4f9917e9p-1022,
0x1.1eaf0f8601c5ap-1022,
0x1.1f549e0df780fp-1022,
0x1.1fd353818739ep-1022,
0x1.209fbb1de2f79p-1022,
0x1.1ffa2c95ed3c4p-1022,
0x1.202105aa533eap-1022,
0x1.20c6943248f9fp-1022,
0x1.2078e2097cf53p-1022,
0x1.214549a5d8b2ep-1022,
0x1.2211b14234709p-1022,
0x1.216c22ba3eb54p-1022,
0x1.2192fbcea4b7ap-1022,
0x1.22388a569a72fp-1022,
0x1.21ead82dce6e3p-1022,
0x1.22de18de902e4p-1022,
0x1.2304f1f2f630ap-1022,
0x1.22b73fca2a2bep-1022,
0x1.23aa807aebebfp-1022,
0x1.235cce521fe73p-1022,
0x1.2383a76685e99p-1022,
0x1.24500f02e1a74p-1022,
0x1.242935ee7ba4ep-1022,
0x1.251c769f3d64fp-1022,
0x1.24cec47671603p-1022,
0x1.257452fe671b8p-1022,
0x1.24f59d8ad7629p-1022,
0x1.259b2c12cd1dep-1022,
0x1.25c2052733204p-1022,
0x1.2640ba9ac2d93p-1022,
0x1.26e64922b8948p-1022,
0x1.266793af28db9p-1022,
0x1.268e6cc38eddfp-1022,
0x1.270d22371e96ep-1022,
0x1.2733fb4b84994p-1022,
0x1.28583f470a0d8p-1022,
0x1.27b2b0bf14523p-1022,
0x1.27d989d37a549p-1022,
0x1.287f185b700fep-1022,
0x1.280062e7e056fp-1022,
0x1.28a5f16fd6124p-1022,
0x1.2924a6e365cb3p-1022,
0x1.294b7ff7cbcd9p-1022,
0x1.29f10e7fc188ep-1022,
0x1.2972590c31cffp-1022,
0x1.2a17e794278b4p-1022,
0x1.29ca356b5b868p-1022,
0x1.2b6304a41301ep-1022,
0x1.2abd761c1d469p-1022,
0x1.2a969d07b7443p-1022,
0x1.2b89ddb879044p-1022,
0x1.2b3c2b8facff8p-1022,
0x1.2be1ba17a2badp-1022,
0x1.2c2f6c406ebf9p-1022,
0x1.2cfbd3dcca7d4p-1022,
0x1.2c08932c08bd3p-1022,
0x1.2cae21b3fe788p-1022,
0x1.2cd4fac8647aep-1022,
0x1.2d53b03bf433dp-1022,
0x1.2e6dca011bf64p-1022,
0x1.2d7a89505a363p-1022,
0x1.2e2017d84ff18p-1022,
0x1.2da16264c0389p-1022,
0x1.2ec5a66045acdp-1022,
0x1.2e46f0ecb5f3ep-1022,
0x1.2eec7f74abaf3p-1022,
0x1.30379c849725dp-1022,
0x1.2f920dfca16a8p-1022,
0x1.2f13588911b19p-1022,
0x1.2fb8e711076cep-1022,
0x1.305e7598fd283p-1022,
0x1.30854ead632a9p-1022,
0x1.31040420f2e38p-1022,
0x1.312add3558e5ep-1022,
0x1.30dd2b0c8ce12p-1022,
0x1.31d06bbd4ea13p-1022,
0x1.31f744d1b4a39p-1022,
0x1.3275fa45445c8p-1022,
0x1.31a992a8e89edp-1022,
0x1.334261e1a01a3p-1022,
0x1.329cd359aa5eep-1022,
0x1.324f2130de5a2p-1022,
0x1.33693af6061c9p-1022,
0x1.331b88cd3a17dp-1022,
0x1.340ec97dfbd7ep-1022,
0x1.33c117552fd32p-1022,
0x1.33e7f06995d58p-1022,
0x1.348d7ef18b90dp-1022,
0x1.3580bfa24d50ep-1022,
0x1.34b45805f1933p-1022,
0x1.35330d79814c2p-1022,
0x1.3559e68de74e8p-1022,
0x1.35ff7515dd09dp-1022,
0x1.36a5039dd2c52p-1022,
0x1.36264e2a430c3p-1022,
0x1.374a9225c8807p-1022,
0x1.36cbdcb238c78p-1022,
0x1.37716b3a2e82dp-1022,
0x1.36f2b5c69ec9ep-1022,
0x1.3816f9c2243e2p-1022,
0x1.3798444e94853p-1022,
0x1.383dd2d68a408p-1022,
0x1.3988efe675b72p-1022,
0x1.38e3615e7ffbdp-1022,
0x1.3864abeaf042ep-1022,
0x1.390a3a72e5fe3p-1022,
0x1.38bc884a19f97p-1022,
0x1.39afc8fadbb98p-1022,
0x1.3a555782d174dp-1022,
0x1.39d6a20f41bbep-1022,
0x1.3a7c309737773p-1022,
0x1.3b21bf1f2d328p-1022,
0x1.3a2e7e6e6b727p-1022,
0x1.3afae60ac7302p-1022,
0x1.3c93b5437eab8p-1022,
0x1.3bee26bb88f03p-1022,
0x1.3ba07492bceb7p-1022,
0x1.3c46031ab2a6cp-1022,
0x1.3bc74da722eddp-1022,
0x1.3c6cdc2f18a92p-1022,
0x1.3d601cdfda693p-1022,
0x1.3d126ab70e647p-1022,
0x1.3db7f93f041fcp-1022,
0x1.3d3943cb7466dp-1022,
0x1.3dded2536a222p-1022,
0x1.3e05ab67d0248p-1022,
0x1.3e8460db5fdd7p-1022,
0x1.3f29ef635598cp-1022,
0x1.3eab39efc5dfdp-1022,
0x1.3f50c877bb9b2p-1022,
0x1.3ed213042be23p-1022,
0x1.3f77a18c219d8p-1022,
0x1.3ff656ffb1567p-1022,
0x1.409be587a711cp-1022,
0x1.401d30141758dp-1022,
0x1.40c2be9c0d142p-1022,
0x1.41684d2402cf7p-1022,
0x1.40e997b073168p-1022,
0x1.4141740f9ccd1p-1022,
0x1.418f263868d1dp-1022,
0x1.4234b4c05e8d2p-1022,
0x1.425b8dd4c48f8p-1022,
0x1.420ddbabf88acp-1022,
0x1.43011c5cba4adp-1022,
0x1.42b36a33ee461p-1022,
0x1.42da434854487p-1022,
0x1.43a6aae4b0062p-1022,
0x1.43cd83f916088p-1022,
0x1.447312810bc3dp-1022,
0x1.437fd1d04a03cp-1022,
0x1.442560583fbf1p-1022,
0x1.444c396ca5c17p-1022,
0x1.453f7a1d67818p-1022,
0x1.44f1c7f49b7ccp-1022,
0x1.4597567c91381p-1022,
0x1.4518a109017f2p-1022,
0x1.45be2f90f73a7p-1022,
0x1.45e508a55d3cdp-1022,
0x1.4663be18ecf5cp-1022,
0x1.47094ca0e2b11p-1022,
0x1.47aedb28d86c6p-1022,
0x1.468a972d52f82p-1022,
0x1.473025b548b37p-1022,
0x1.4756fec9aeb5dp-1022,
0x1.47d5b43d3e6ecp-1022,
0x1.487b42c5342a1p-1022,
0x1.47fc8d51a4712p-1022,
0x1.48a21bd99a2c7p-1022,
0x1.48c8f4ee002edp-1022,
0x1.4947aa618fe7cp-1022,
0x1.4920d14d29e56p-1022,
0x1.496e8375f5ea2p-1022,
0x1.4a1411fdeba57p-1022,
0x1.4ab9a085e160cp-1022,
0x1.4a3aeb1251a7dp-1022,
0x1.49ed38e985a31p-1022,
0x1.4a92c7717b5e6p-1022,
0x1.4ae0799a47632p-1022,
0x1.4b8608223d1e7p-1022,
0x1.4b5f2f0dd71c1p-1022,
0x1.4c04bd95ccd76p-1022,
0x1.4c526fbe98dc2p-1022,
0x1.4c2b96aa32d9cp-1022,
0x1.4caa4c1dc292bp-1022,
0x1.4cd1253228951p-1022,
0x1.4dc465e2ea552p-1022,
0x1.4cf7fe468e977p-1022,
0x1.4d76b3ba1e506p-1022,
0x1.4e1c4242140bbp-1022,
0x1.4d9d8cce8452cp-1022,
0x1.4e431b567a0e1p-1022,
0x1.4ee8a9de6fc96p-1022,
0x1.4e69f46ae0107p-1022,
0x1.4f0f82f2d5cbcp-1022,
0x1.4f8e38666584bp-1022,
0x1.4f365c073bce2p-1022,
0x1.4fb5117acb871p-1022,
0x1.505aa002c1426p-1022,
0x1.4fdbea8f31897p-1022,
0x1.508179172744cp-1022,
0x1.51cc962712bb6p-1022,
0x1.5127079f1d001p-1022,
0x1.51002e8ab6fdbp-1022,
0x1.514de0b383027p-1022,
0x1.51f36f3b78bdcp-1022,
0x1.51a5bd12acb9p-1022,
0x1.5298fdc36e791p-1022,
0x1.527224af0876bp-1022,
0x1.5365655fca36cp-1022,
0x1.52bfd6d7d47b7p-1022,
0x1.5317b336fe32p-1022,
0x1.533e8c4b64346p-1022,
0x1.54d75b841bafcp-1022,
0x1.5431ccfc25f47p-1022,
0x1.53e41ad359efbp-1022,
0x1.540af3e7bff21p-1022,
0x1.5489a95b4fabp-1022,
0x1.54b0826fb5ad6p-1022,
0x1.55a3c320776d7p-1022,
0x1.555610f7ab68bp-1022,
0x1.557cea0c116b1p-1022,
0x1.55fb9f7fa124p-1022,
0x1.564951a86d28cp-1022,
0x1.56c8071bfce1bp-1022,
0x1.5622789407266p-1022,
0x1.56eee03062e41p-1022,
0x1.576d95a3f29dp-1022,
0x1.57946eb8589f6p-1022,
0x1.5839fd404e5abp-1022,
0x1.57bb47ccbea1cp-1022,
0x1.5813242be8585p-1022,
0x1.58df8bc84416p-1022,
0x1.5860d654b45d1p-1022,
0x1.59abf3649fd3bp-1022,
0x1.590664dcaa186p-1022,
0x1.592d3df1101acp-1022,
0x1.59851a5039d15p-1022,
0x1.59d2cc7905d61p-1022,
0x1.5a785b00fb916p-1022,
0x1.5a9f34156193cp-1022,
0x1.5a5181ec958fp-1022,
0x1.5b44c29d574f1p-1022,
0x1.5af710748b4a5p-1022,
0x1.5bea51254d0a6p-1022,
0x1.5b1de988f14cbp-1022,
0x1.5cb6b8c1a8c81p-1022,
0x1.5bc37810e708p-1022,
0x1.5c690698dcc35p-1022,
0x1.5c8fdfad42c5bp-1022,
0x1.5d0e9520d27eap-1022,
0x1.5d356e353881p-1022,
0x1.5d5c47499e836p-1022,
0x1.5ddafcbd2e3c5p-1022,
0x1.5e808b4523f7ap-1022,
0x1.5e01d5d1943ebp-1022,
0x1.5e28aee5fa411p-1022,
0x1.5ea7645989fap-1022,
0x1.5ff281697570ap-1022,
0x1.5f4cf2e17fb55p-1022,
0x1.5ece3d6deffc6p-1022,
0x1.5f73cbf5e5b7bp-1022,
0x1.5f9aa50a4bba1p-1022,
0x1.60195a7ddb73p-1022,
0x1.60bee905d12e5p-1022,
0x1.6040339241756p-1022,
0x1.60e5c21a3730bp-1022,
0x1.618b50a22cecp-1022,
0x1.6164778dc6e9ap-1022,
0x1.61b229b692ee6p-1022,
0x1.6257b83e88a9bp-1022,
0x1.6230df2a22a75p-1022,
0x1.62d66db21862ap-1022,
0x1.6514cb72c5995p-1022,
0x1.64bcef139be2cp-1022,
0x1.64c71949f9949p-1022,
0x1.646f3ceacfdep-1022,
0x1.6417608ba6277p-1022,
0x1.64218ac203d94p-1022,
0x1.63c9ae62da22bp-1022,
0x1.637bfc3a0e1dfp-1022,
0x1.63241fdae4676p-1022,
0x1.65627d9b919e1p-1022,
0x1.656ca7d1ef4fep-1022,
0x1.67a0db5c3ed4cp-1022,
0x1.67ab05929c869p-1022,
0x1.6753293372dp-1022,
0x1.6705770aa6cb4p-1022,
0x1.66ad9aab7d14bp-1022,
0x1.665fe882b10ffp-1022,
0x1.66080c2387596p-1022,
0x1.65ba59fabb54ap-1022,
0x1.69e9635349bd4p-1022,
0x1.699186f42006bp-1022,
0x1.6943d4cb5401fp-1022,
0x1.68ebf86c2a4b6p-1022,
0x1.68f622a287fd3p-1022,
0x1.689e46435e46ap-1022,
0x1.684669e434901p-1022,
0x1.6850941a9241ep-1022,
0x1.67f8b7bb688b5p-1022,
0x1.6a37157c15c2p-1022,
0x1.6c75733cc2f8bp-1022,
0x1.6bcfe4b4cd3d6p-1022,
0x1.6c27c113f6f3fp-1022,
0x1.6bda0eeb2aef3p-1022,
0x1.6b82328c0138ap-1022,
0x1.6b2a562cd7821p-1022,
0x1.6adca4040b7d5p-1022,
0x1.6b3480633533ep-1022,
0x1.6a84c7a4e1c6cp-1022,
0x1.6a8ef1db3f789p-1022,
0x1.6e661ed4a42aap-1022,
0x1.6e0e42757a741p-1022,
0x1.6e186cabd825ep-1022,
0x1.6dc0904cae6f5p-1022,
0x1.6d72de23e26a9p-1022,
0x1.6d1b01c4b8b4p-1022,
0x1.6ccd4f9becaf4p-1022,
0x1.6f0bad5c99e5fp-1022,
0x1.6eb3d0fd702f6p-1022,
0x1.6ebdfb33cde13p-1022,
0x1.6f595f8565eabp-1022,
0x1.714a0b1d471cap-1022,
0x1.70fc58f47b17ep-1022,
0x1.70a47c9551615p-1022,
0x1.7056ca6c855c9p-1022,
0x1.6ffeee0d5ba6p-1022,
0x1.6fb13be48fa14p-1022,
0x1.6f6389bbc39c8p-1022,
0x1.72e2da55fe98p-1022,
0x1.7197bd4613216p-1022,
0x1.7295282d32934p-1022,
0x1.723d4bce08dcbp-1022,
0x1.71a1e77c70d33p-1022,
0x1.72477604668e8p-1022,
0x1.71ef99a53cd7fp-1022,
0x1.733ab6b5284e9p-1022,
0x1.738868ddf4535p-1022,
0x1.73e0453d1e09ep-1022,
0x1.766c552697455p-1022,
0x1.75c6c69ea18ap-1022,
0x1.761ea2fdcb409p-1022,
0x1.75d0f0d4ff3bdp-1022,
0x1.75791475d5854p-1022,
0x1.75213816abcebp-1022,
0x1.752b624d09808p-1022,
0x1.747ba98eb6136p-1022,
0x1.74d385eddfc9fp-1022,
0x1.742df765ea0eap-1022,
0x1.7485d3c513c53p-1022,
0x1.7769c00db6b73p-1022,
0x1.7711e3ae8d00ap-1022,
0x1.76c43185c0fbep-1022,
0x1.785d00be78774p-1022,
0x1.7805245f4ec0bp-1022,
0x1.78aab2e7447cp-1022,
0x1.780f4e95ac728p-1022,
0x1.77b7723682bbfp-1022,
0x1.78b4dd1da22ddp-1022,
0x1.7b40ed071b694p-1022,
0x1.7ae910a7f1b2bp-1022,
0x1.7a9b5e7f25adfp-1022,
0x1.79028f466e329p-1022,
0x1.7af33ade4f648p-1022,
0x1.7a4dac5659a93p-1022,
0x1.79f5cff72ff2ap-1022,
0x1.79a81dce63edep-1022,
0x1.7950416f3a375p-1022,
0x1.795a6ba597e92p-1022,
0x1.7be67b8f11249p-1022,
0x1.7b8e9f2fe76ep-1022,
0x1.7b98c966451fdp-1022,
0x1.7d7f4ac7c89ffp-1022,
0x1.7cd9bc3fd2e4ap-1022,
0x1.7c8c0a1706dfep-1022,
0x1.7d31989efc9b3p-1022,
0x1.7c3e57ee3adb2p-1022,
0x1.7c342db7dd295p-1022,
0x1.7dd72726f2568p-1022,
0x1.801584e79f8d3p-1022,
0x1.7fbda88875d6ap-1022,
0x1.7fc7d2bed3887p-1022,
0x1.7f6ff65fa9d1ep-1022,
0x1.7f181a00801b5p-1022,
0x1.7f224436ddcd2p-1022,
0x1.7eca67d7b4169p-1022,
0x1.7e728b788a6p-1022,
0x1.7e24d94fbe5b4p-1022,
0x1.7e7cb5aee811dp-1022,
0x1.806337106b91fp-1022,
0x1.81fc0649230d5p-1022,
0x1.82a194d118c8ap-1022,
0x1.8206307f80bf2p-1022,
0x1.8253e2a84cc3ep-1022,
0x1.82abbf07767a7p-1022,
0x1.81ae542057089p-1022,
0x1.815677c12d52p-1022,
0x1.8108c598614d4p-1022,
0x1.8160a1f78b03dp-1022,
0x1.80bb136f95488p-1022,
0x1.84ea1cc823b12p-1022,
0x1.84924068f9fa9p-1022,
0x1.84448e402df5dp-1022,
0x1.83ecb1e1043f4p-1022,
0x1.839effb8383a8p-1022,
0x1.834723590e83fp-1022,
0x1.82f97130427f3p-1022,
0x1.84dff291c5ff5p-1022,
0x1.8537cef0efb5ep-1022,
0x1.87287a88d0e7dp-1022,
0x1.86d09e29a7314p-1022,
0x1.87762cb19cec9p-1022,
0x1.862b0fa1b175fp-1022,
0x1.85858119bbbaap-1022,
0x1.858fab50196c7p-1022,
0x1.8682ec00db2c8p-1022,
0x1.85dd5d78e5713p-1022,
0x1.863539d80f27cp-1022,
0x1.8966d8497e1e8p-1022,
0x1.890efbea5467fp-1022,
0x1.89192620b219cp-1022,
0x1.88c149c188633p-1022,
0x1.88696d625eacap-1022,
0x1.88739798bc5e7p-1022,
0x1.881bbb3992a7ep-1022,
0x1.87ce0910c6a32p-1022,
0x1.89b48a724a234p-1022,
0x1.89beb4a8a7d51p-1022,
0x1.8a0c66d173d9dp-1022,
0x1.8bf2e832f759fp-1022,
0x1.8c4ac49221108p-1022,
0x1.8bfd1269550bcp-1022,
0x1.8ba5360a2b553p-1022,
0x1.8b4d59ab019eap-1022,
0x1.8affa7823599ep-1022,
0x1.8b5783e15f507p-1022,
0x1.8a5a18fa3fde9p-1022,
0x1.8ab1f55969952p-1022,
0x1.8c9876baed154p-1022,
0x1.8ca2a0f14ac71p-1022,
0x1.8d95e1a20c872p-1022,
0x1.8d3e0542e2d09p-1022,
0x1.8cf0531a16cbdp-1022,
0x1.8e892252ce473p-1022,
0x1.8de393cad88bep-1022,
0x1.8ee0feb1f7fdcp-1022,
0x1.8ed6d47b9a4bfp-1022,
0x1.8e3b702a02427p-1022,
0x1.916d0e9b71393p-1022,
0x1.90c780137b7dep-1022,
0x1.911f5c72a5347p-1022,
0x1.9079cdeaaf792p-1022,
0x1.8f2eb0dac4028p-1022,
0x1.8fd43f62b9bddp-1022,
0x1.8f7c630390074p-1022,
0x1.9021f18b85c29p-1022,
0x1.902c1bc1e3746p-1022,
0x1.8f868d39edb91p-1022,
0x1.926a798290ab1p-1022,
0x1.92129d2366f48p-1022,
0x1.91bac0c43d3dfp-1022,
0x1.91c4eafa9aefcp-1022,
0x1.935dba33526b2p-1022,
0x1.93ab6c5c1e6fep-1022,
0x1.9310080a86666p-1022,
0x1.92604f4c32f94p-1022,
0x1.9305ddd428b49p-1022,
0x1.92b82bab5cafdp-1022,
0x1.940348bb48267p-1022,
0x1.95e9ca1ccba69p-1022,
0x1.9641a67bf55d2p-1022,
0x1.959c17f3ffa1dp-1022,
0x1.95f3f45329586p-1022,
0x1.95443b94d5eb4p-1022,
0x1.954e65cb339d1p-1022,
0x1.94a8d7433de1cp-1022,
0x1.94f6896c09e68p-1022,
0x1.9450fae4142b3p-1022,
0x1.96e73503eb187p-1022,
0x1.968f58a4c161ep-1022,
0x1.969982db1f13bp-1022,
0x1.97da75b4acd88p-1022,
0x1.9880043ca293dp-1022,
0x1.98cdb6656e989p-1022,
0x1.9734e72cb71d3p-1022,
0x1.978cc38be0d3cp-1022,
0x1.98325213d68f1p-1022,
0x1.98d7e09bcc4a6p-1022,
0x1.9b163e5c79811p-1022,
0x1.9abe61fd4fca8p-1022,
0x1.9a70afd483c5cp-1022,
0x1.9a18d3755a0f3p-1022,
0x1.997344ed6453ep-1022,
0x1.99cb214c8e0a7p-1022,
0x1.9a22fdabb7c1p-1022,
0x1.992592c4984f2p-1022,
0x1.997d6f23c205bp-1022,
0x1.9b63f0854585dp-1022,
0x1.9cfcbfbdfd013p-1022,
0x1.9d549c1d26b7cp-1022,
0x1.9caf0d9530fc7p-1022,
0x1.9c615b6c64f7bp-1022,
0x1.9bb1a2ae118a9p-1022,
0x1.9bbbcce46f3c6p-1022,
0x1.9c5731360745ep-1022,
0x1.9c097f0d3b412p-1022,
0x1.9da24e45f2bc8p-1022,
0x1.9d06e9f45ab3p-1022,
0x1.9fead63cfda5p-1022,
0x1.9f92f9ddd3ee7p-1022,
0x1.9f3b1d7eaa37ep-1022,
0x1.9f4547b507e9bp-1022,
0x1.9eed6b55de332p-1022,
0x1.9e9fb92d122e6p-1022,
0x1.9e47dccde877dp-1022,
0x1.9dfa2aa51c731p-1022,
0x1.9fe0ac069ff33p-1022,
0x1.a0388865c9a9cp-1022,
0x1.a1d1579e81252p-1022,
0x1.a22933fdaadbbp-1022,
0x1.a21f09c74d29ep-1022,
0x1.a0863a8e95ae8p-1022,
0x1.a09064c4f3605p-1022,
0x1.a12bc9168b69dp-1022,
0x1.a0de16edbf651p-1022,
0x1.a276e62676e07p-1022,
0x1.a183a575b5206p-1022,
0x1.a46791be58126p-1022,
0x1.a40fb55f2e5bdp-1022,
0x1.a3c2033662571p-1022,
0x1.a36a26d738a08p-1022,
0x1.a2c4984f42e53p-1022,
0x1.a374510d96525p-1022,
0x1.a2cec285a097p-1022,
0x1.a31c74ae6c9bcp-1022,
0x1.a4b543e724172p-1022,
0x1.a50d20464dcdbp-1022,
0x1.a74b7e06fb046p-1022,
0x1.a6f3a1a7d14ddp-1022,
0x1.a55ad26f19d27p-1022,
0x1.a6a5ef7f05491p-1022,
0x1.a64e131fdb928p-1022,
0x1.a6fdcbde2effap-1022,
0x1.a60060f70f8dcp-1022,
0x1.a5a88497e5d73p-1022,
0x1.a6583d5639445p-1022,
0x1.a5b2aece4389p-1022,
0x1.a799302fc7092p-1022,
0x1.a8969b16e67bp-1022,
0x1.a83ebeb7bcc47p-1022,
0x1.a7f10c8ef0bfbp-1022,
0x1.a931ff687e848p-1022,
0x1.a8e44d3fb27fcp-1022,
0x1.a93c299edc365p-1022,
0x1.a9d78df0743fdp-1022,
0x1.a989dbc7a83b1p-1022,
0x1.a9e1b826d1f1ap-1022,
0x1.ac15ebb121768p-1022,
0x1.ac2015e77f285p-1022,
0x1.abc839885571cp-1022,
0x1.ac6dc8104b2d1p-1022,
0x1.aa2f6a4f9df66p-1022,
0x1.ab22ab005fb67p-1022,
0x1.ab7a875f896dp-1022,
0x1.aa7d1c7869fb2p-1022,
0x1.aad4f8d793b1bp-1022,
0x1.aa8746aec7acfp-1022,
0x1.ad6b32f76a9efp-1022,
0x1.ad13569840e86p-1022,
0x1.acbb7a391731dp-1022,
0x1.acc5a46f74e3ap-1022,
0x1.ad6108c10ced2p-1022,
0x1.ae5e73a82c5fp-1022,
0x1.adb8e52036a3bp-1022,
0x1.ae06974902a87p-1022,
0x1.aeac25d0f863cp-1022,
0x1.b0ea8391a59a7p-1022,
0x1.b1425ff0cf51p-1022,
0x1.b0f4adc8034c4p-1022,
0x1.af040230221a5p-1022,
0x1.b09cd168d995bp-1022,
0x1.b044f509afdf2p-1022,
0x1.b04f1f400d90fp-1022,
0x1.aff742e0e3da6p-1022,
0x1.afa990b817d5ap-1022,
0x1.af9f6681ba23dp-1022,
0x1.af51b458ee1f1p-1022,
0x1.b1e7ee78c50c5p-1022,
0x1.b19012199b55cp-1022,
0x1.b28352ca5d15dp-1022,
0x1.b28d7d00bac7ap-1022,
0x1.b2db2f2986cc6p-1022,
0x1.b235a0a191111p-1022,
0x1.b380bdb17c87bp-1022,
0x1.b3330b88b082fp-1022,
0x1.b328e15252d12p-1022,
0x1.b3ce6fda488c7p-1022,
0x1.b3d89a10a63e4p-1022,
0x1.b60ccd9af5c32p-1022,
0x1.b616f7d15374fp-1022,
0x1.b5bf1b7229be6p-1022,
0x1.b57169495db9ap-1022,
0x1.b5198cea34031p-1022,
0x1.b473fe623e47cp-1022,
0x1.b4264c397243p-1022,
0x1.b4cbdac167fe5p-1022,
0x1.b664a9fa1f79bp-1022,
0x1.b6b25c22eb7e7p-1022,
0x1.b757eaaae139cp-1022,
0x1.b70a38821535p-1022,
0x1.b6bc865949304p-1022,
0x1.b7fd7932d6f51p-1022,
0x1.b855559200abap-1022,
0x1.b76214e13eeb9p-1022,
0x1.b7afc70a0af05p-1022,
0x1.b8a307baccb06p-1022,
0x1.ba93b352ade25p-1022,
0x1.ba3bd6f3842bcp-1022,
0x1.ba460129e1dd9p-1022,
0x1.b9ee24cab827p-1022,
0x1.b996486b8e707p-1022,
0x1.b9a072a1ec224p-1022,
0x1.b8fae419f666fp-1022,
0x1.b9489642c26bbp-1022,
0x1.bae1657b79e71p-1022,
0x1.bb3941daa39dap-1022,
0x1.bb86f4036fa26p-1022,
0x1.bbded0629958fp-1022,
0x1.bc2c828b655dbp-1022,
0x1.bc7a34b431627p-1022,
0x1.bc845eea8f144p-1022,
0x1.bd1fc33c271dcp-1022,
0x1.bcd211135b19p-1022,
0x1.bd29ed7284cf9p-1022,
0x1.bd779f9b50d45p-1022,
0x1.bf106ed4084fbp-1022,
0x1.bec2bcab3c4afp-1022,
0x1.be6ae04c12946p-1022,
0x1.bdc551c41cd91p-1022,
0x1.bdcf7bfa7a8aep-1022,
0x1.be1d2e23468fap-1022,
0x1.bf684b3332064p-1022,
0x1.bfb5fd5bfe0bp-1022,
0x1.c003af84ca0fcp-1022,
0x1.c00dd9bb27c19p-1022,
0x1.c05b8be3f3c65p-1022,
0x1.c0b368431d7cep-1022,
0x1.c14ecc94b5866p-1022,
0x1.c0a93e0cbfcb1p-1022,
0x1.c158f6cb13383p-1022,
0x1.c1011a6be981ap-1022,
0x1.c1f45b1cab41bp-1022,
0x1.c1a6a8f3df3cfp-1022,
0x1.c24c377bd4f84p-1022,
0x1.c299e9a4a0fdp-1022,
0x1.c38d2a5562bd1p-1022,
0x1.c397548bc06eep-1022,
0x1.c33f782c96b85p-1022,
0x1.c2e79bcd6d01cp-1022,
0x1.c2f1c603cab39p-1022,
0x1.c3e506b48c73ap-1022,
0x1.c43ce313b62a3p-1022,
0x1.c432b8dd58786p-1022,
0x1.c48a953c822efp-1022,
0x1.c4d847654e33bp-1022,
0x1.c53023c477ea4p-1022,
0x1.c716a525fb6a6p-1022,
0x1.c57dd5ed43efp-1022,
0x1.c5d5b24c6da59p-1022,
0x1.c720cf5c591c3p-1022,
0x1.c623647539aa5p-1022,
0x1.c671169e05af1p-1022,
0x1.c6c8f2fd2f65ap-1022,
0x1.c67b40d46360ep-1022,
0x1.c76e81852520fp-1022,
0x1.c814100d1adc4p-1022,
0x1.c7bc33adf125bp-1022,
0x1.c7c65de44ed78p-1022,
0x1.c861c235e6e1p-1022,
0x1.c8b99e9510979p-1022,
0x1.c90750bddc9c5p-1022,
0x1.c9acdf45d257ap-1022,
0x1.c95f2d1d0652ep-1022,
0x1.c9fa916e9e5c6p-1022,
0x1.ca04bba4fc0e3p-1022,
0x1.cbeb3d067f8e5p-1022,
0x1.cb45ae7e89d3p-1022,
0x1.cc431965a944ep-1022,
0x1.ca526dcdc812fp-1022,
0x1.caf7fc55bdce4p-1022,
0x1.caa01ff69417bp-1022,
0x1.cb9d8addb3899p-1022,
0x1.caaa4a2cf1c98p-1022,
0x1.cc90cb8e7549ap-1022,
0x1.cce8a7ed9f003p-1022,
0x1.ccde7db7414e6p-1022,
0x1.cd365a166b04fp-1022,
0x1.cd8e367594bb8p-1022,
0x1.cd840c3f3709bp-1022,
0x1.cddbe89e60c04p-1022,
0x1.ce33c4fd8a76dp-1022,
0x1.ce817726567b9p-1022,
0x1.ce299ac72cc5p-1022,
0x1.cecf294f22805p-1022,
0x1.d10d870fcfb7p-1022,
0x1.d117b1462d68dp-1022,
0x1.d0bfd4e703b24p-1022,
0x1.d067f887d9fbbp-1022,
0x1.d01a465f0df6fp-1022,
0x1.cf74b7d7183bap-1022,
0x1.cf2705ae4c36ep-1022,
0x1.d07222be37ad8p-1022,
0x1.cfcc943641f23p-1022,
0x1.d165636ef96d9p-1022,
0x1.d1b31597c5725p-1022,
0x1.d1bd3fce23242p-1022,
0x1.d20af1f6ef28ep-1022,
0x1.d258a41fbb2dap-1022,
0x1.d2b0807ee4e43p-1022,
0x1.d2fe32a7b0e8fp-1022,
0x1.d34be4d07cedbp-1022,
0x1.d3560f06da9f8p-1022,
0x1.d3a3c12fa6a44p-1022,
0x1.d5946cc787d63p-1022,
0x1.d53c90685e1fap-1022,
0x1.d3f1735872a9p-1022,
0x1.d49701e068645p-1022,
0x1.d4eede3f921aep-1022,
0x1.d4a12c16c6162p-1022,
0x1.d3fb9d8ed05adp-1022,
0x1.d4494fb79c5f9p-1022,
0x1.d5e21ef053dafp-1022,
0x1.d639fb4f7d918p-1022,
0x1.d687ad7849964p-1022,
0x1.d6d55fa1159bp-1022,
0x1.d6df89d7734cdp-1022,
0x1.d72d3c003f519p-1022,
0x1.d77aee290b565p-1022,
0x1.d785185f69082p-1022,
0x1.d7d2ca88350cep-1022,
0x1.d8207cb10111ap-1022,
0x1.d82aa6e75ec37p-1022,
0x1.d87859102ac83p-1022,
0x1.da112848e2439p-1022,
0x1.d9b94be9b88dp-1022,
0x1.d9c37620163edp-1022,
0x1.d8c60b38f6ccfp-1022,
0x1.d96b99c0ec884p-1022,
0x1.d91de79820838p-1022,
0x1.da5eda71ae485p-1022,
0x1.da6904a80bfa2p-1022,
0x1.dab6b6d0d7feep-1022,
0x1.db0468f9a403ap-1022,
0x1.db0e933001b57p-1022,
0x1.db5c4558cdba3p-1022,
0x1.dba9f78199befp-1022,
0x1.dc01d3e0c3758p-1022,
0x1.dc4f86098f7a4p-1022,
0x1.dca76268b930dp-1022,
0x1.dd42c6ba513a5p-1022,
0x1.dcf5149185359p-1022,
0x1.dd4cf0f0aeec2p-1022,
0x1.de8de3ca3cb0fp-1022,
0x1.dd9aa3197af0ep-1022,
0x1.de980e009a62cp-1022,
0x1.de4031a170ac3p-1022,
0x1.dde8554246f5ap-1022,
0x1.ddf27f78a4a77p-1022,
0x1.dee5c02966678p-1022,
0x1.df337252326c4p-1022,
0x1.df8b4eb15c22dp-1022,
0x1.dfd900da28279p-1022,
0x1.e030dd3951de2p-1022,
0x1.e07e8f621de2ep-1022,
0x1.e0cc418ae9e7ap-1022,
0x1.e0d66bc147997p-1022,
0x1.e171d012dfa2fp-1022,
0x1.e1241dea139e3p-1022,
0x1.e2175e9ad55e4p-1022,
0x1.e17bfa493d54cp-1022,
0x1.e1c9ac7209598p-1022,
0x1.e22188d133101p-1022,
0x1.e26f3af9ff14dp-1022,
0x1.e314c981f4d02p-1022,
0x1.e2bced22cb199p-1022,
0x1.e3627baac0d4ep-1022,
0x1.e3b02dd38cd9ap-1022,
0x1.e3ba5809ea8b7p-1022,
0x1.e4080a32b6903p-1022,
0x1.e45fe691e046cp-1022,
0x1.e455bc5b8294fp-1022,
0x1.e4ad98baac4b8p-1022,
0x1.e4fb4ae378504p-1022,
0x1.e5057519d6021p-1022,
0x1.e739a8a42586fp-1022,
0x1.e5532742a206dp-1022,
0x1.e5a0d96b6e0b9p-1022,
0x1.e64667f363c6ep-1022,
0x1.e5f8b5ca97c22p-1022,
0x1.e6ebf67b59823p-1022,
0x1.e69e44528d7d7p-1022,
0x1.e743d2da8338cp-1022,
0x1.e79185034f3d8p-1022,
0x1.e7e9616278f41p-1022,
0x1.e7df372c1b424p-1022,
0x1.e837138b44f8dp-1022,
0x1.e88eefea6eaf6p-1022,
0x1.e884c5b410fd9p-1022,
0x1.e8dca2133ab42p-1022,
0x1.e92a543c06b8ep-1022,
0x1.e982309b306f7p-1022,
0x1.e9cfe2c3fc743p-1022,
0x1.ec0e4084a9aaep-1022,
0x1.eb68b1fcb3ef9p-1022,
0x1.ea1d94ecc878fp-1022,
0x1.ebc08e5bdda62p-1022,
0x1.eb1affd3e7eadp-1022,
0x1.eb72dc3311a16p-1022,
0x1.eac32374be344p-1022,
0x1.ea27bf23262acp-1022,
0x1.ea75714bf22f8p-1022,
0x1.eacd4dab1be61p-1022,
0x1.ec661ce3d3617p-1022,
0x1.ecb3cf0c9f663p-1022,
0x1.ed0bab6bc91ccp-1022,
0x1.eda70fbd61264p-1022,
0x1.ed595d9495218p-1022,
0x1.edb139f3bed81p-1022,
0x1.edfeec1c8adcdp-1022,
0x1.ee56c87bb4936p-1022,
0x1.ee4c9e4556e19p-1022,
0x1.eea47aa480982p-1022,
0x1.f095263c61ca1p-1022,
0x1.f03d49dd38138p-1022,
0x1.ef97bb5542583p-1022,
0x1.efef97b46c0ecp-1022,
0x1.eef22ccd4c9cep-1022,
0x1.eefc5703aa4ebp-1022,
0x1.ef4a092c76537p-1022,
0x1.f0e2d8652dcedp-1022,
0x1.f1308a8df9d39p-1022,
0x1.f13ab4c457856p-1022,
0x1.f18866ed238a2p-1022,
0x1.f1e0434c4d40bp-1022,
0x1.f1d61915ef8eep-1022,
0x1.f22df57519457p-1022,
0x1.f27ba79de54a3p-1022,
0x1.f285d1d442fcp-1022,
0x1.f2d383fd0f00cp-1022,
0x1.f3213625db058p-1022,
0x1.f379128504bc1p-1022,
0x1.f511e1bdbc377p-1022,
0x1.f4ba055e9280ep-1022,
0x1.f41476d69cc59p-1022,
0x1.f4c42f94f032bp-1022,
0x1.f3c6c4add0c0dp-1022,
0x1.f46c5335c67c2p-1022,
0x1.f41ea10cfa776p-1022,
0x1.f55f93e6883c3p-1022,
0x1.f569be1ce5eep-1022,
0x1.f5b77045b1f2cp-1022,
0x1.f605226e7df78p-1022,
0x1.f65cfecda7ae1p-1022,
0x1.f6aab0f673b2dp-1022,
0x1.f7028d559d696p-1022,
0x1.f79df1a73572ep-1022,
0x1.f7503f7e696e2p-1022,
0x1.f7a81bdd9324bp-1022,
0x1.f7f5ce065f297p-1022,
0x1.f843802f2b2e3p-1022,
0x1.f84daa6588ep-1022,
0x1.f98e9d3f16a4dp-1022,
0x1.f89b5c8e54e4cp-1022,
0x1.f940eb164aa01p-1022,
0x1.f8e90eb720e98p-1022,
0x1.f8f338ed7e9b5p-1022,
0x1.f9e6799e405b6p-1022,
0x1.fa342bc70c602p-1022,
0x1.fa81ddefd864ep-1022,
0x1.fa8c08263616bp-1022,
0x1.fad9ba4f021b7p-1022,
0x1.fb276c77ce203p-1022,
0x1.fb3196ae2bd2p-1022,
0x1.fb7f48d6f7d6cp-1022,
0x1.fbd72536218d5p-1022,
0x1.fc728987b996dp-1022,
0x1.fbccfaffc3db8p-1022,
0x1.fc24d75eed921p-1022,
0x1.fcca65e6e34d6p-1022,
0x1.fd18180faf522p-1022,
0x1.fd6ff46ed908bp-1022,
0x1.fe0b58c071123p-1022,
0x1.fe1582f6cec4p-1022,
0x1.fdbda697a50d7p-1022,
0x1.fe63351f9ac8cp-1022,
0x1.feb0e74866cd8p-1022,
0x1.febb117ec47f5p-1022,
0x1.ff08c3a790841p-1022,
0x1.ff60a006ba3aap-1022,
0x1.ff5675d05c88dp-1022,
0x1.ffae522f863f6p-1022,
0x1.fffc045852442p-1022,
};

/* check worst cases in [2^-1022, 2^-106) from those for the
   binade 2^-1022 <= x < 2^-1021 */
static void
check_tiny (void)
{
  for (int i = 0; i < N_WORST; i++)
  {
    double x = T[i];
    for (int e = -1021; e <= -106; e++)
    {
      // check binade 2^(e-1) <= |x| < 2^e
      check (ldexp (x, e + 1021));
      check (ldexp (-x, e + 1021));
    }
  }
}

int
main (int argc, char *argv[])
{
  while (argc >= 2)
    {
      if (strcmp (argv[1], "--rndn") == 0)
        {
          rnd = 0;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndz") == 0)
        {
          rnd = 1;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndu") == 0)
        {
          rnd = 2;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndd") == 0)
        {
          rnd = 3;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--verbose") == 0)
        {
          verbose = 1;
          argc --;
          argv ++;
        }
      else
        {
          fprintf (stderr, "Error, unknown option %s\n", argv[1]);
          exit (1);
        }
    }
  ref_init ();
  ref_fesetround (rnd);

#define N 1000000UL /* total number of tests */

  unsigned int seed = getpid ();
  srand (seed);

#pragma omp parallel for
  for (uint64_t n = 0; n < N; n++)
  {
    ref_init ();
    ref_fesetround (rnd);
    double x;
    do x = get_random (); while (x <= -1.0);
    check (x);
  }

  printf ("Checking tiny worst cases\n");
  check_tiny ();

  return 0;
}