<template>
  <v-container fluid grid-list-md fill mt-0>
    <v-tabs
     v-model="activeTab"
     color="cyan"
     dark
     slider-color="yellow"
    >
      <v-tab :key="0">Gyro Calibration</v-tab>
      <v-tab :key="1">Accelerometer Calibration</v-tab> 
      <v-tab :key="2">Magnetometer Calibration</v-tab>

      <v-tab-item :key="0">
        <v-card flat>
          <v-card-text>Gyro Calibration</v-card-text>
        </v-card>
      </v-tab-item>

      <v-tab-item :key="1">
        <v-card flat>
          <v-card-text>Accelerometer Calibration</v-card-text>
        </v-card>
      </v-tab-item>

      <v-tab-item :key="2">
        <v-card flat>
          <v-card-text>Magnetometer Calibration</v-card-text>
        </v-card>
      </v-tab-item>
    </v-tabs>
  </v-container>
</template>

<script>
  export default {
    data () {
      return {
        activeTab: 0
      }
    }
  }
</script>

<style scoped>
</style>
