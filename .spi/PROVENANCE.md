# SPI provenance

The SPI fork starts from OSDU community commit
`a4903fdea082e0641ae59d2a0611406870bed47a`.

The Azure implementation in ADO branch `m26-master` at
`e18783139c67fab6acd20e24ab76e95142071eee` was used as behavioral prior art
for Workload Identity and the existing Azure authentication plugin. The ADO
OpenZGY binary bundle and private dependency sources are not included.

The native build downloads these public, immutable inputs:

- Ubuntu 22.04 image index:
  `sha256:0a9887931b0e38af2e3487ed39e29436b0535c4f3f52e57745baaf79d1898956`
- OpenZGY Jammy bundle `0.2.1253`:
  `sha256:445982e6f3da2aa43c5ab3ce39608facd63f052e2d25d4e97e4d33e82b03d0c9`
- Azure SDK for C++ commit
  `4b913e073e50466e8b2d674e65234e7b66d7db49`:
  `sha256:5b273a6fe3ea7342624d4567c1804fae146808cde92f112155b72676fa0d2d74`
