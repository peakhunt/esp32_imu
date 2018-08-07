<template>
  <v-dialog v-model="showDialog" persistent max-width="600px">
    <v-card class="text-xs-center">
      <v-card-title>
        <span class="headline">Magnetometer Calibration In Progress</span>
      </v-card-title>

      <v-card-text>
        <v-container fluid>
          <v-layout row wrap>
            <v-flex xs12>
              <sphere-fitting-view :width="400" :height="400" ref="uncalibated"></sphere-fitting-view>
            </v-flex>
          </v-layout>
        </v-container>
      </v-card-text>
    </v-card>
  </v-dialog>
</template>

<script>
  import SphereFittingView from '@/components/SphereFittingView'
  import {ImuComm} from '@/imu_comm_ws'

  export default {
    name: 'MagCalibrationDialog',
    props: ['showDialog'],
    components: {
      SphereFittingView
    },
    methods: {
      onImuOrientation (o) {
        if (this.calibrationStarted === false && o.mode === 3) {
          console.log('**** turning on mag calibration started ***')
          this.calibrationStarted = true
        }

        var r

        r = o.raw
        this.$refs.uncalibated.push_point({
          x: r.mx / 50,
          y: r.my / 50,
          z: r.mz / 50
        })

        if (this.calibrationStarted && o.mode === 0) {
          console.log('**** firing mag calibration complete ***')
          this.calibrationStarted = false
          ImuComm.$off('imuMagnetometer', this.onImuOrientation)

          this.$emit('magCalFinished')
        }
      }
    },
    watch: {
      showDialog (n, o) {
        if (this.showDialog === true) {
          this.calibrationStarted = false
          this.$refs.uncalibated.clear_points()
          ImuComm.$on('imuMagnetometer', this.onImuOrientation)

          ImuComm.startMagCalibration((err, rsp) => {
            if (err) {
              console.log(err)
              return
            }
            console.log('*** started calibration ***')
          })
        }
      }
    },
    data () {
      return {
        calibrationStarted: false
      }
    }
  }
</script>

<style scoped>
</style>
