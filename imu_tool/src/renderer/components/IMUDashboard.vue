<template>
  <v-container fluid grid-list-md mt-0 fill-height>
    <v-layout row wrap>
      <v-flex xs12>
        <v-card>
          <v-card-title>
            <h2>IMU Dashbord</h2>
          </v-card-title>
          <compass :value="value" :width="width" :height="height"> </compass>
        </v-card>
      </v-flex>
    </v-layout>
  </v-container>
 
</template>

<script>
  import Compass from '@/components/Compass'

  export default {
    name: 'IMUDashboard',
    components: { Compass },
    methods: {
      onImuOrientation (o) {
        console.log('compass orient: ' + o.data.yaw)
        this.value = o.data.yaw
      }
    },
    created () {
      this.$root.$children[0].$on('imuOrientation', this.onImuOrientation)
    },
    beforeDestroy () {
      this.$root.$children[0].$off('imuOrientation', this.onImuOrientation)
    },
    data () {
      return {
        value: 0,
        width: 300,
        height: 300
      }
    }
  }
</script>

<style scoped>
</style>
