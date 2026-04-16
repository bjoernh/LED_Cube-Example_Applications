function Base.uv_readcb(handle::Ptr{Cvoid}, nread::Cssize_t, buf::Ptr{Cvoid})
    stream_unknown_type = Base.@handle_as handle Base.LibuvStream
    nrequested = ccall(:jl_uv_buf_len, Csize_t, (Ptr{Cvoid},), buf)
    function readcb_specialized(stream::Base.LibuvStream, nread::Int, nrequested::UInt)
        lock(stream.cond)
        if nread < 0
            if nread == Base.UV_ENOBUFS && nrequested == 0
                # remind the client that stream.buffer is full
                notify(stream.cond)
            elseif nread == Base.UV_EOF # libuv called uv_stop_reading already
                if stream.status != Base.StatusClosing
                    stream.status = Base.StatusEOF
                    notify(stream.cond)
                    if stream isa Base.TTY
                        # stream can still be used by reseteof (or possibly write)
                    elseif !(stream isa Base.PipeEndpoint) && ccall(:uv_is_writable, Cint, (Ptr{Cvoid},), stream.handle) != 0
                        # stream can still be used by write
                    else
                        # underlying stream is no longer useful: begin finalization
                        ccall(:jl_close_uv, Cvoid, (Ptr{Cvoid},), stream.handle)
                        stream.status = Base.StatusClosing
                    end
                end
            else
                stream.readerror = Base._UVError("read", nread)
                notify(stream.cond)
                # This is a fatal connection error
                ccall(:jl_close_uv, Cvoid, (Ptr{Cvoid},), stream.handle)
                stream.status = Base.StatusClosing
            end
        else
            Base.notify_filled(stream.buffer, nread)
            notify(stream.cond)
        end
        unlock(stream.cond)

        # Stop background reading when
        # 1) there's nobody paying attention to the data we are reading
        # 2) we have accumulated a lot of unread data OR
        # 3) we have an alternate buffer that has reached its limit.
        if stream.status == Base.StatusPaused ||
                       (stream.status == Base.StatusActive &&
            ((bytesavailable(stream.buffer) >= stream.throttle) ||
             (bytesavailable(stream.buffer) >= stream.buffer.maxsize)))
            # save cycles by stopping kernel notifications from arriving
            ccall(:uv_read_stop, Cint, (Ptr{Cvoid},), stream)
            stream.status = Base.StatusOpen
        end
        nothing
    end
    if stream_unknown_type isa TCPSocket
        return readcb_specialized(stream_unknown_type::TCPSocket, nread, nrequested)
    else
        return readcb_specialized(stream_unknown_type, nread, nrequested)
    end
    nothing
end

Base.Experimental.entrypoint(Base.uv_readcb, (Ptr{Cvoid}, Cssize_t, Ptr{Cvoid}))