from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("MNIST_data/", one_hot=True)
 
import tensorflow as tf
x = tf.placeholder(tf.float32, [None, 784])
 
W = tf.Variable(tf.zeros([784, 10]))
b = tf.Variable(tf.zeros([10]))
 
y = tf.nn.softmax(tf.matmul(x, W) + b)
 
y_ = tf.placeholder(tf.float32, [None, 10])
 
cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_ * tf.log(y), reduction_indices=[1]))
 
train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)
 
sess = tf.InteractiveSession()
 
tf.global_variables_initializer().run()
 
for _ in range(1000):
  batch_xs, batch_ys = mnist.train.next_batch(100)
  sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys})
 
correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
 
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
 
print(sess.run(accuracy, feed_dict={x: mnist.test.images, y_: mnist.test.labels}))

#************************************************************************************
#************************************************************************************

from sklearn import model_selection
from sklearn import metrics
import tensorflow as tf


# Load dataset.
iris = tf.contrib.learn.datasets.load_dataset('iris')

x_train, x_test, y_train, y_test = model_selection.train_test_split(
  iris.data, iris.target, test_size=0.2, random_state=42)

# Build 3 layer DNN with 10, 20, 10 units respectively.

feature_columns = tf.contrib.learn.infer_real_valued_columns_from_input(
  x_train)

classifier = tf.contrib.learn.DNNClassifier(
  feature_columns=feature_columns, hidden_units=[10, 20, 10], n_classes=3)

# Fit and predict.
classifier.fit(x_train, y_train, steps=200)
predictions = list(classifier.predict(x_test, as_iterable=True))
score = metrics.accuracy_score(y_test, predictions)
print('Accuracy: {0:f}'.format(score))