#ifndef TAXIVIS_ASYNC_TASK_HPP
#define TAXIVIS_ASYNC_TASK_HPP
#include <QObject>
#include <QPointer>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QDebug>
#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>

struct WorkCancelled {};
struct Cancellation {
    std::shared_ptr<std::atomic_bool> flag = std::make_shared<std::atomic_bool>(false);
    bool cancelled() const { return flag->load(std::memory_order_relaxed); }
    void cancel() const { flag->store(true, std::memory_order_relaxed); }
    void check() const { if (cancelled()) throw WorkCancelled(); }
};

// GUI-thread controller. Workers capture only values, never the controller or
// widgets. Destruction cancels without waiting; context-bound delivery prevents
// callbacks into destroyed views. At most one active + one latest pending job.
template<class Result> class LatestTask : public QObject {
    struct Outcome { std::shared_ptr<Result> value; QString error; };
    using Work = std::function<Result(const Cancellation&)>;
    Work pending;
    std::function<void(Result)> apply;
    Cancellation token;
    bool running = false;
    bool busy_ = false;
    void busy(bool value) {
        if (busy_ == value) return;
        busy_ = value;
        if (onBusy) onBusy(value);
    }
    void start() {
        if (!pending) { busy(false); return; }
        Work work = std::move(pending);
        pending = Work();
        token = Cancellation();
        Cancellation captured = token;
        running = true;
        auto watcher = new QFutureWatcher<Outcome>(this);
        connect(watcher, &QFutureWatcher<Outcome>::finished, this, [this, watcher, captured]() {
            Outcome result = watcher->result();
            watcher->deleteLater();
            running = false;
            if (!captured.cancelled()) {
                QPointer<LatestTask> alive(this);
                if (result.value) { auto ready=apply; ready(std::move(*result.value)); }
                else if (!result.error.isEmpty()) {
                    if (onError) onError(result.error);
                    else qWarning() << "Background task failed:" << result.error;
                }
                if (!alive) return;
            }
            if (!running) start(); // apply may itself submit a request
        });
        watcher->setFuture(QtConcurrent::run([work, captured]() {
            Outcome result;
            try {
                captured.check();
                Result value = work(captured);
                captured.check();
                result.value = std::make_shared<Result>(std::move(value));
            } catch (const WorkCancelled &) {
            } catch (const std::exception &error) {
                result.error = QString::fromUtf8(error.what());
            } catch (...) {
                result.error = QStringLiteral("Unknown worker error");
            }
            return result;
        }));
    }
public:
    std::function<void(bool)> onBusy;
    std::function<void(const QString&)> onError;
    explicit LatestTask(QObject *parent = nullptr) : QObject(parent) {}
    ~LatestTask() override { token.cancel(); }
    bool isBusy() const { return busy_; }
    void cancel() { token.cancel(); pending = Work(); busy(false); }
    void submit(Work work, std::function<void(Result)> ready) {
        token.cancel();
        pending = std::move(work);
        apply = std::move(ready);
        busy(true);
        if (!running) start();
    }
};
#endif
