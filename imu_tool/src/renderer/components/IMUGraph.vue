<template>
  <v-container fluid grid-list-md mt-0 fill-height>
    <v-layout row wrap>
      <v-flex xs12>
        <v-card>
          <v-card-title>
            <h2>IMU Signals</h2>
          </v-card-title>

          <line-chart id="wrapper" :chartData="data" :options="options" ref="lineGraph"
           v-dragged="onDragged" v-wheel="onWheel">
          </line-chart>
        </v-card>
      </v-flex>
    </v-layout>
  </v-container>
 
</template>

<script>
  import LineChart from '@/components/LineChart'
  import dateFormat from 'dateformat'

  export default {
    name: 'IMUGraph',
    components: { LineChart, dateFormat },
    methods: {
      onDragged ({ el, deltaX, deltaY, offsetX, offsetY, clientX, clientY, first, last }) {
      },
      onWheel (e) {
      },
      onImuOrientation (o) {
        console.log('*** onImuOrientation ***')
        var d = new Date()
        var signals = ['roll', 'pitch', 'yaw', 'ax', 'ay', 'az', 'gx', 'gy', 'gz', 'mx', 'my', 'mz']
        var ndx = 0

        d = dateFormat(d, 'HH:MM:ss')

        this.data.labels.push(d)
        this.data.labels_accumulated.push(d)

        if (this.data.labels.length > this.plotCfg.maxVisible) {
          this.data.labels.splice(0, 1)
        }

        if (this.data.labels_accumulated.length > this.plotCfg.maxKeep) {
          this.data.labels_accumulated.splice(0, 1)
        }

        signals.forEach((v) => {
          this.data.datasets[ndx].data.push(o.data[v])
          this.data.datasets[ndx].data_accumulated.push(o.data[v])

          if (this.data.datasets[ndx].data.length > this.plotCfg.maxVisible) {
            this.data.datasets[ndx].data.splice(0, 1)
          }

          if (this.data.datasets[ndx].data_accumulated.length > this.plotCfg.maxKeep) {
            this.data.datasets[ndx].data_accumulated.splice(0, 1)
          }
          ndx++
        })

        this.$refs['lineGraph'].refresh()
      }
    },
    created () {
      this.$root.$children[0].$on('imuOrientation', this.onImuOrientation)
    },
    beforeDestroy () {
      this.$root.$children[0].$off('imuOrientation', this.onImuOrientation)
    },
    data () {
      var configOptions = {
        responsive: true,
        animation: false,
        maintainAspectRatio: false,
        elements: {
          point: {
            radius: 0.5
          }
        },
        scales: {
          xAxes: [
            {
              grindLines: {
                display: true
              },
              scaleLabel: {
                display: true,
                labelString: 'time'
              }
            }
          ],
          yAxes: [
            {
              grindLines: {
                display: true
              },
              scaleLabel: {
                display: true,
                labelString: 'value'
              }
            }
          ]
        }
      }

      return {
        plotCfg: {
          maxVisible: 180,
          maxKeep: 7200
        },
        data: {
          labels: [],
          labels_accumulated: [],
          datasets: [
            {
              label: 'Roll',
              fill: false,
              borderColor: 'blue',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'Pitch',
              fill: false,
              borderColor: 'brown',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'Yaw',
              fill: false,
              borderColor: 'red',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'AX',
              fill: false,
              borderColor: 'green',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'AY',
              fill: false,
              borderColor: 'purple',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'AZ',
              fill: false,
              borderColor: 'blue',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'GX',
              fill: false,
              borderColor: 'cyan',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'GY',
              fill: false,
              borderColor: 'teal',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'GZ',
              fill: false,
              borderColor: 'yellow',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'MX',
              fill: false,
              borderColor: 'orange',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'MY',
              fill: false,
              borderColor: 'white',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            },
            {
              label: 'MZ',
              fill: false,
              borderColor: 'blue-grey',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              showLine: true
            }
          ]
        },
        options: configOptions
      }
    }
  }
</script>

<style scoped>
#wrapper {
  height: 70vh;
}
</style>
