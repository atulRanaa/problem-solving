library(e1071)
train <- read.csv("train.csv")
test <- read.csv("test.csv")


m1 <- svm(as.factor(hand) ~ .,data =train , gamma= 0.1, cost = 1, kernel='linear')
pred <- predict(m1)
table(pred, train$hand, dnn=c("Prediction", "Actual"))
