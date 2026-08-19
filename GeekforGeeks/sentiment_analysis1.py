import tweepy
from textblob import TextBlob

# Step 1 - Authenticate
consumer_key= 'eUYIWnmgkJ8qj5GGfJenojIqR'
consumer_secret= 'ktHMKwAjDdxsILooxrK8awdwEmgAzBRB7FWtxImwJQuspFm39P'

access_token='3815186718-jDm0o3GzcABtJ9PJmpAcKxN661KNdwwHrXRLAlI'
access_token_secret='ew62OaApFrEZJt1wZtw29XTc5QiqJFL6Z1A2xPFD47goD'

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth)

#Step 3 - Retrieve Tweets
public_tweets = api.search('howtotrainyourdragon3')



#CHALLENGE - Instead of printing out each tweet, save each Tweet to a CSV file
#and label each one as either 'positive' or 'negative', depending on the sentiment 
#You can decide the sentiment polarity threshold yourself


for tweet in public_tweets:
    print(tweet.text)
    
    #Step 4 Perform Sentiment Analysis on Tweets
    analysis = TextBlob(tweet.text)
    print(analysis.sentiment)
print("")
