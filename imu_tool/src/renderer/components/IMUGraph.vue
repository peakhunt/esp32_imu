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
        if (this.playingRealtime) {
          return
        }

        if (deltaX !== undefined) {
          this.shift_visible(deltaX * 5)
        }
      },
      onWheel (e) {
        if (this.playingRealtime) {
          return
        }

        var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)))

        var shift = (this.plotCfg.maxVisible / 4) * (delta < 0 ? -1 : 1)
        this.shift_visible(shift)
        e.preventDefault()
      },
      shift_visible (delta) {
        this.visibleStart += delta
        this.adjust_offset_ndx_and_reload()
      },
      adjust_offset_ndx_and_reload () {
        if (this.visibleStart >= this.data.labels_accumulated.length - this.plotCfg.maxVisible) {
          this.visibleStart = this.data.labels_accumulated.length - this.plotCfg.maxVisible
        }

        if (this.visibleStart < 0) {
          this.visibleStart = 0
        }

        console.log('visibleStart:' + this.visibleStart + ', total data:' + this.data.labels_accumulated.length +
            ', max visible:' + this.plotCfg.maxVisible)

        this.reload_visible_data()
      },
      reload_visible_data () {
        var i
        var self = this
        var numSamples
        var ndx

        this.data.labels = []

        numSamples = Math.min(this.plotCfg.maxVisible, this.data.labels_accumulated.length)

        console.log('reloading visible: ' + numSamples + ', from: ' + this.visibleStart)

        for (i = 0; i < numSamples; i++) {
          this.data.labels[i] = this.data.labels_accumulated[this.visibleStart + i]
        }

        ndx = 0
        this.data.datasets.forEach(function (element) {
          element.data[ndx] = []

          for (i = 0; i < numSamples; i++) {
            element.data[i] = element.data_accumulated[self.visibleStart + i]
          }
          ndx += 1
        })
        this.$refs['lineGraph'].refresh()
      },
      /*
      onImuOrientation (o) {
        console.log('*** onImuOrientation ***')

        var d = new Date()
        var signals = ['roll', 'pitch', 'yaw', 'ax', 'ay', 'az', 'gx', 'gy', 'gz', 'mx', 'my', 'mz']
        var ndx = 0
        var self = this

        d = dateFormat(d, 'HH:MM:ss')

        this.data.labels_buffered.push(d)

        signals.forEach((v) => {
          self.data.datasets[ndx].data_buffered.push(o.data[v])
          ndx++
        })

        if (this.data.labels_buffered.length >= this.plotCfg.numDataToBuffer) {
          self.plotGraph()
        }
      },
      plotGraph () {
        var self = this

        self.data.labels_buffered.forEach((d) => {
          self.data.labels.push(d)
          self.data.labels_accumulated.push(d)

          if (self.data.labels.length > self.plotCfg.maxVisible) {
            self.data.labels.splice(0, 1)
            if (self.data.labels_accumulated.length <= self.plotCfg.maxKeep) {
              self.visibleStart += 1
            }
          }

          if (self.data.labels_accumulated.length > self.plotCfg.maxKeep) {
            self.data.labels_accumulated.splice(0, 1)
          }
        })
        self.data.labels_buffered = []

        self.data.datasets.forEach((set) => {
          set.data_buffered.forEach((d) => {
            set.data.push(d)
            set.data_accumulated.push(d)

            if (set.data.length > self.plotCfg.maxVisible) {
              set.data.splice(0, 1)
            }

            if (set.data_accumulated.length > self.plotCfg.maxKeep) {
              set.data_accumulated.splice(0, 1)
            }
          })
          set.data_buffered = []
        })

        this.$refs['lineGraph'].refresh()
      },
      */
      onImuOrientation (o) {
        console.log('*** onImuOrientation ***')

        var d = new Date()
        var signals = ['roll', 'pitch', 'yaw', 'ax', 'ay', 'az', 'gx', 'gy', 'gz', 'mx', 'my', 'mz']
        var ndx = 0
        var self = this

        d = dateFormat(d, 'HH:MM:ss')

        self.data.labels[self.data.labels.length] = d
        self.data.labels_accumulated[self.data.labels_accumulated.length] = d

        if (self.data.labels.length > self.plotCfg.maxVisible) {
          self.data.labels.splice(0, 1)
          if (self.data.labels_accumulated.length <= self.plotCfg.maxKeep) {
            self.visibleStart += 1
          }
        }

        if (self.data.labels_accumulated.length > self.plotCfg.maxKeep) {
          self.data.labels_accumulated.splice(0, 1)
        }

        signals.forEach((v) => {
          self.data.datasets[ndx].data[self.data.datasets[ndx].data.length] = o.data[v]

          self.data.datasets[ndx].data_accumulated[self.data.datasets[ndx].data_accumulated.length] = o.data[v]

          if (self.data.datasets[ndx].data.length > self.plotCfg.maxVisible) {
            self.data.datasets[ndx].data.splice(0, 1)
          }

          if (self.data.datasets[ndx].data_accumulated.length > self.plotCfg.maxKeep) {
            self.data.datasets[ndx].data_accumulated.splice(0, 1)
          }
          ndx++
        })

        self.count++

        if (self.count >= 5) {
          self.$refs['lineGraph'].refresh()
          self.count = 0
        }
      },
      clearData () {
        this.data.labels = []
        this.data.labels_accumulated = []
        this.visibleStart = 0

        this.data.datasets.forEach(function (element) {
          element.data = []
          element.data_accumulated = []
          element.data_buffered = []
        })
      },
      onConnected () {
        this.playingRealtime = true
        this.clearData()
      },
      onDisconnected () {
        this.playingRealtime = false
      }
    },
    created () {
      this.$root.$children[0].$on('imuOrientation', this.onImuOrientation)
      this.$root.$children[0].$on('onConnected', this.onConnected)
      this.$root.$children[0].$on('onDisconnected', this.onDisconnected)

      this.playingRealtime = !this.$root.$children[0].isStopped
    },
    beforeDestroy () {
      this.$root.$children[0].$off('imuOrientation', this.onImuOrientation)
      this.$root.$children[0].$off('onConnected', this.onConnected)
      this.$root.$children[0].$off('onDisconnected', this.onDisconnected)
    },
    data () {
      var configOptions = {
        playingRealtime: false,
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
          maxKeep: 7200,
          numDataToBuffer: 10
        },
        count: 0,
        visibleStart: 0,
        data: {
          labels: [],
          labels_accumulated: [],
          labels_buffered: [],
          datasets: [
            {
              label: 'Roll',
              fill: false,
              borderColor: 'blue',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'Pitch',
              fill: false,
              borderColor: 'brown',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'Yaw',
              fill: false,
              borderColor: 'red',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'AX',
              fill: false,
              borderColor: 'green',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'AY',
              fill: false,
              borderColor: 'purple',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'AZ',
              fill: false,
              borderColor: 'blue',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'GX',
              fill: false,
              borderColor: 'cyan',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'GY',
              fill: false,
              borderColor: 'teal',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'GZ',
              fill: false,
              borderColor: 'yellow',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'MX',
              fill: false,
              borderColor: 'orange',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'MY',
              fill: false,
              borderColor: 'white',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
              showLine: true
            },
            {
              label: 'MZ',
              fill: false,
              borderColor: 'blue-grey',
              pointRadius: 1.5,
              data: [],
              data_accumulated: [],
              data_buffered: [],
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
