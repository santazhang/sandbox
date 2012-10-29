require 'rubygems'
require 'json'
require 'thread'
require 'socket'
require 'pp'

class Server

  def initialize
    @func_map = {}
  end

  def def func, &block
    @func_map[func] = block
  end

  def start host, port
    @socket = TCPServer.open host, port
    loop do
      serve @socket.accept
    end
  end

  def serve clnt
    Thread.new do
      loop do
        json_req = clnt.gets
        break unless json_req
        req = JSON.load json_req
        if @func_map.has_key? req['req']
          val = @func_map[req['req']].call *req['args']
          ret = {'id' => req['id'], 'val' => val}
        else
          ret = {'id' => req['id'], 'failure' => "method '#{req['req']}' not found"}
        end
        clnt.write "#{ret.to_json}\n"
      end
    end
  end

end


class Client

  def initialize host, port
    @future_map = {}
    @mutex = Mutex.new
    @queue = Queue.new
    @conn = TCPSocket.new host, port

    Thread.new do
      while true do
        p = @queue.pop
        @conn.write "#{p.to_json}\n"
      end
    end

    Thread.new do
      loop do
        ret = @conn.gets
        break unless ret
        r = JSON.load ret
        @mutex.synchronize do
          f = @future_map[r['id']]
          if r.has_key? 'val'
            f.set r['val']
          else
            f.set_failure r['failure']
          end
          @future_map.delete r['id']
        end
      end
    end
  end

private

  def rpc_call m, *a, &b
    f = Future.new b
    p = {:req => m, :args => a, :id => f.object_id}
    @mutex.synchronize { @future_map[f.object_id] = f }
    @queue << p
    return f
  end

  def method_missing m, *a, &b
    if m.to_s.start_with? "async_"
      rpc_m = m.to_s[6..-1]
      return rpc_call rpc_m, *a, &b
    elsif m.to_s.start_with? "sync_"
      rpc_m = m.to_s[5..-1]
      return rpc_call(rpc_m, *a, &b).get
    else
      super
    end
  end

end


class Future

  class DoNothingCallback
    def initialize val
      @val = val;
    end
    def success &block; end
    def failure &block; end
  end

  class SuccessCallback < DoNothingCallback
    def success &block
      yield @val
    end
  end

  class FailureCallback < DoNothingCallback
    def failure &block
      yield @val
    end
  end

  def initialize block
    @block = block
    @val = nil
    @ready = false
    @mutex = Mutex.new
    @cond = ConditionVariable.new
  end

  def get
    ret = nil
    @mutex.synchronize do
      while @ready == false
        @cond.wait @mutex
      end
      ret = @val
    end
    return ret
  end

  def set val
    @mutex.synchronize do
      @val = val
      @ready = true
    end
    if @block != nil
      @block.call SuccessCallback.new(@val)
    end
    @cond.broadcast
  end

  def set_failure failure
    @mutex.synchronize do
      @val = nil
      @ready = true
    end
    if @block != nil
      @block.call FailureCallback.new(failure)
    end
    @cond.broadcast
  end

end

