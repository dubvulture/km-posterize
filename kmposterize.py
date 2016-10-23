import numpy as np
import cv2
import re
from cffi import FFI


KM_FLOAT32 = 0
KM_INT32 = 1
KM_UINT8 = 2



class Image(object):


	def __init__(self, input_image, greyscale=False, matrix=None, depth=KM_UINT8):
		"""
		input_image: An image.
		greyscale: A boolean indicating whether to read the image on a greyscale or not
		matrix: An array representing the image.
		depth: An integer indicating each channel depth
		"""
		np_type = {
			KM_FLOAT32: np.float32,
			KM_INT32: np.int32,
			KM_UINT8: np.uint8

		}.get(depth, None)

		if type(matrix) is np.ndarray:
			np_type == matrix.dtype
		elif matrix == None:
			pass
		else:
			raise ValueError('Only numpy arrays are allowed.')

		if np_type not in [np.uint8, np.int32, np.float32]:
			raise ValueError('Invalid type for numpy array.')

		self.ffi = FFI()
		with open('./src/wrapper.h', 'r') as f:
			self.ffi.cdef(f.read())
		self.lib = self.ffi.dlopen('./wrapper.so')

		self.input_image = input_image
		self.greyscale = greyscale
		self.np_type = np_type
		self.depth = depth
		self.matrix = matrix or self.read_image()
		(self.height, self.width) = self.matrix.shape[:2]


	def read_image(self):
		"""
		Read input_image using OpenCV (uses L*a*b* color space)
		"""
		image = cv2.imread(self.input_image, 0 if self.greyscale else 1)
		if not self.greyscale:
			image = cv2.cvtColor(image, cv2.COLOR_BGR2LAB)

		if self.np_type == np.float32:
			image = image.astype(self.np_type) / np.iinfo(image.dtype).max 
	
		return image


	def posterize(self, levels):
		"""
		Posterize image using KMeans with levels number of clusters
		"""
		points = self.height * self.width
		dim = 1 if self.greyscale else 3
		image = self.matrix.flatten()

		# Cast numpy array to void *
		c_matrix = self.ffi.cast('void *', image.ctypes.data)
		# Apply KMeans and get centroids
		c_centroids = self.lib.kmeans(c_matrix, points, dim, levels, self.depth)
		# Paint posterized image
		c_posterized = self.lib.assign_color(c_matrix, c_centroids, points, dim, levels, self.depth)

		# Cast posterized image to it original type
		pointer_type = {
			np.float32: 'float *',
			np.int32: 'int32_t *',
			np.uint8: 'uint8_t *'
		}[self.np_type]
		c_posterized_cast = self.ffi.cast(pointer_type, c_posterized)

		# Initialize buffer from which we'll read the new numpy array
		c_buffer = self.ffi.buffer(c_posterized_cast, points*dim*self.matrix.dtype.itemsize)
		posterized = np.frombuffer(c_buffer, dtype=self.np_type)

		if self.np_type == np.float32:
			posterized = (posterized * 255).astype(np.uint8)

		# Reshape it to the original size
		if self.greyscale:
			posterized = np.reshape(posterized, (self.height, self.width))
		else:
			posterized = np.reshape(posterized, (self.height, self.width, 3))
			posterized = cv2.cvtColor(posterized, cv2.COLOR_LAB2BGR)

		output = re.sub('(?P<pre>.*)(?P<format>\..*?)', '\g<pre>_output\g<format>', self.input_image)
		cv2.imwrite(output, posterized)
