<template>
  <v-card flat>
    <v-container fluid grid-list-md text-xs-center style="min-width: 900px;">
      <v-layout row align-center justify-center>
        <v-flex xs6>
          <p>Raw/Unaligned</p>
          <sphere-fitting-view :width="400" :height="400" ref="uncalibated"></sphere-fitting-view>
        </v-flex>
        
        <v-flex xs6>
          <p>Calibration Applied/Sensor Aligned</p>
          <sphere-fitting-view :width="400" :height="400" ref="calibrated"></sphere-fitting-view>
        </v-flex>
      </v-layout>
    </v-container>
  </v-card>
</template>

<script>
  import SphereFittingView from '@/components/SphereFittingView'
  import {ImuComm} from '@/imu_comm'

  export default {
    name: 'MagCalibrationVue',
    components: {
      SphereFittingView
    },
    methods: {
      onImuOrientation (o) {
        var r, c

        r = o.raw
        c = o.cal

        this.$refs.uncalibated.push_point({
          x: r.mx / 50,
          y: r.my / 50,
          z: r.mz / 50
        })

        this.$refs.calibrated.push_point({
          x: c.mx / 50,
          y: c.my / 50,
          z: c.mz / 50
        })
      }
    },
    created () {
      ImuComm.$on('imuMagnetometer', this.onImuOrientation)
    },
    beforeDestroy () {
      ImuComm.$off('imuMagnetometer', this.onImuOrientation)
    }
  }
</script>

<style scoped>
</style>
