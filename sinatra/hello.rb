#!/usr/bin/ruby

require 'rubygems'
require 'sinatra'

def worker
  counter = 1
  loop do
    `touch #{counter}`
    sleep 1
    counter = counter + 1
  end
end

thread = Thread.new {worker}

get '/' do
  "Hi!"
end

get '/hello' do
  "Hello to yourself! :D"
end

get '/email/:any' do
  "Email? " + params[:any]
end

get '/:req' do
  "I see you are seeking for " + params[:req]
end

get /.*/ do
  "What are you doing?"
end

