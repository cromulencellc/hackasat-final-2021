# Opensatkit Docker Repository
Docker image repository for HAS2 Project.

Includes:
- CFS Builder Images (Docker image to build cfs missions)
    - RTEMS (builds CDH, RTEMS, x86)
        - Patched greth and ping rtems for custom CDH board
        - See rtems_patched/README.md for more information
    - ARMHF (build ADCS)
    - see osk/cfs/README.md for more information
- Cosmos Image
    - Run Ball Aerospace Cosmos in Docker container
