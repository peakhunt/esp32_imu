import Vue from 'vue'

const ImuCommModeOrientation = 0
const ImuCommModeMagnetometer = 1

export const ImuComm = new Vue({
  methods: {
    getOrientationData () {
      var self = this

      if (this.ws === null) {
        return
      }

      var url = '/imu/orientation'

      self.numRequest++

      self.ws.onmessage = function (e) {
        var msg = e.data

        self.numSuccess++

        if (self.isStopped === true) {
          return
        }

        // console.log('orientation: ' + msg)
        self.$emit('imuOrientation', JSON.parse(msg))

        if (self.wait !== 0) {
          self.timer = setTimeout(() => {
            self.getIMUData()
          }, self.wait)
        } else {
          self.getIMUData()
        }
      }
      self.ws.send(url)
    },
    getMagData () {
      var self = this
      var url = '/imu/mag_data'

      self.numRequest++

      self.ws.onmessage = function (e) {
        var msg = e.data

        // console.log('mag data: ' + msg)
        self.numSuccess++

        if (self.isStopped === true) {
          return
        }

        self.$emit('imuMagnetometer', JSON.parse(msg))

        if (self.wait !== 0) {
          self.timer = setTimeout(() => {
            self.getIMUData()
          }, self.wait)
        } else {
          self.getIMUData()
        }
      }
      self.ws.send(url)
    },
    getIMUData () {
      switch (this.mode) {
        case ImuCommModeOrientation:
          this.getOrientationData()
          break

        case ImuCommModeMagnetometer:
          this.getMagData()
          break
      }
    },
    start (serverInfo) {
      var self = this

      self.ipAddress = serverInfo.ipAddress
      self.port = serverInfo.port
      self.wait = serverInfo.wait
      self.bufferSize = serverInfo.bufferSize
      self.isStopped = false

      self.$emit('onConnecting')
      self.ws = new WebSocket('ws://' + self.ipAddress + ':' + self.port, 'json')

      self.ws.onopen = function () {
        self.$emit('onConnected')

        console.log('start polling: ' + self.ipAddress + ':' + self.port)
        self.getIMUData()
      }

      self.ws.onerror = function (error) {
        console.log('WebSocket Error ' + error)
      }
    },
    stop () {
      var self = this

      console.log('stop polling: ' + self.ipAddress + ':' + self.port)

      self.$emit('onDisconnected')
      self.isStopped = true
      if (self.timer != null) {
        clearTimeout(self.timer)
        self.timer = null
      }
      self.ws.close()
      self.ws = null
    },
    putOrientationMode () {
      this.mode = ImuCommModeOrientation
    },
    putMagnetometerMode () {
      this.mode = ImuCommModeMagnetometer
    },
    startMagCalibration (callback) {
      var self = this
      var url = 'http://' + self.ipAddress + ':' + self.port + '/imu/mag_calibrate'

      self.$http.post(url,
        {
          dummy: 0
        },
        {
          headers: { 'Content-Type': 'application/json' }
        })
        .then((response) => {
          callback(null, response)
        }, (err) => {
          callback(err, null)
        })
    }
  },
  data: {
    ipAddress: null,
    port: null,
    wait: 0,
    isStopped: true,
    timer: null,
    bufferSize: 5,
    numRequest: 0,
    numSuccess: 0,
    numFail: 0,
    mode: ImuCommModeOrientation,
    ws: null
  }
})
