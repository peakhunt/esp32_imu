<template>
  <div :width="width" :height="height" ref="threeCanvas">
  </div>
</template>

<script>
  // import THREE from 'three'
  const THREE = require('three')
  const OrbitControls = require('three-orbit-controls')(THREE)

  export default {
    name: 'OrientationThree',
    props: ['width', 'height'],
    methods: {
      do_render () {
        this.renderer.render(this.scene, this.camera)
        requestAnimationFrame(this.do_render)
      }
    },
    mounted () {
      this.camera = new THREE.PerspectiveCamera(75, this.width / this.height, 0.1, 1000)
      this.camera.position.set(0, 4, 4)

      this.renderer = new THREE.WebGLRenderer({
        alpha: false
      })
      this.renderer.setSize(this.width, this.height)
      this.$refs.threeCanvas.appendChild(this.renderer.domElement)
      // this.renderer.setClearColor(new THREE.Color(0xffffff, 1.0))

      this.scene = new THREE.Scene()

      this.scene.add(new THREE.AmbientLight(0x7f0000))

      this.light = new THREE.PointLight(0xaa7a40, 2, 1500)
      this.light.position.set(-100, 200, 100)
      this.scene.add(this.light)

      var geometry = new THREE.BoxGeometry(3, 0.5, 5)
      // var material = new THREE.MeshBasicMaterial({color: 0xa3a3a3})
      var material = new THREE.MeshPhongMaterial({color: 0xff6383})
      this.cube = new THREE.Mesh(geometry, material)

      this.scene.add(this.camera)
      this.scene.add(this.cube)

      // this.axisHelper = new THREE.AxisHelper(3.25)
      // this.scene.add(this.axisHelper)

      var origin = new THREE.Vector3(0, 0, 0)

      this.arrowX = new THREE.ArrowHelper(new THREE.Vector3(0, 0, -1), origin, 5, 0xff0000)
      this.arrowY = new THREE.ArrowHelper(new THREE.Vector3(1, 0, 0), origin, 3, 0x00ff00)
      this.arrowZ = new THREE.ArrowHelper(new THREE.Vector3(0, 1, 0), origin, 2.5, 0x0000ff)

      this.scene.add(this.arrowX)
      this.scene.add(this.arrowY)
      this.scene.add(this.arrowZ)

      this.controls = new OrbitControls(this.camera, this.renderer.domElement)

      this.do_render()
    },
    beforeDestroy () {
    },
    data () {
      return {
        camera: null,
        renderer: null,
        scene: null,
        light: null,
        cube: null,
        controls: null,
        axisHelper: null,
        arrowX: null,
        arrowY: null,
        arrowZ: null
      }
    },
    watch: {
    }
  }
</script>

<style scoped>
</style>
