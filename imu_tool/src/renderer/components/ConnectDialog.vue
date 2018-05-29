<template>
  <v-dialog v-model="showDialog" persistent max-wdith="400px">
    <v-card>
      <v-card-title>
        <span class="headline">Connect To IMU</span>
      </v-card-title>
      <v-card-text>
        <v-text-field label="IMU IP Address" v-model.string="ipAddress" required></v-text-field>
        <v-text-field label="IMU IP Address" v-model.number="port" required></v-text-field>
        <v-text-field label="Poll Wait Time" v-model.number="wait" required></v-text-field>
      </v-card-text>

      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn color="blue darken-1" flat @click.native="dismiss()">Cancel</v-btn>
        <v-btn color="blue darken-1" flat :disabled="isInputOK() === false" @click="connect()">Connect</v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
  export default {
    name: 'ConnectDialog',
    props: ['showDialog'],
    methods: {
      dismiss () {
        this.$emit('dismiss')
      },
      connect () {
        this.$emit('connect', { ipAddress: this.ipAddress, port: this.port, wait: this.wait })
      },
      isInputOK () {
        if (this.ipAddress != null && this.port != null) {
          return true
        }
        return false
      }
    },
    watch: {
      showDialog: function (newVal) {
        if (newVal === true) {
          // FIXME initialize
        }
      }
    },
    data () {
      return {
        ipAddress: null,
        port: 80,
        wait: 10
      }
    }
  }
</script>

<style scoped>
</style>
