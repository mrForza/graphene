

if __name__ == '__main__':
    queue_size_array = []
    queue_size = 0
    with open('./cmake-build-debug/dijkstra.log', 'r') as dijkstra_log:
        log_line = dijkstra_log.readline()
        while log_line != '':
            action, vertex = log_line.split(',')
            if action == 'RELAX':
                queue_size += 1
                queue_size_array.append(queue_size)
            elif action == 'COMPLETE' and vertex != 0:
                queue_size -= 1
                queue_size_array.append(queue_size)
            log_line = dijkstra_log.readline()

    print(queue_size_array[1:-1])
